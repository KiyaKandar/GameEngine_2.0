#include "Mesh.h"
#include <limits>
#include <algorithm>

typedef SubMesh* (*processmeshFunc)(aiMesh*, aiScene*);

const aiMatrix4x4	conversion(
	-1, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
);

void Mesh::LoadModel(std::string path)
{
	scene = import.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw runtime_error("Mesh not found");
	}

	globalInverseTransform = scene->mRootNode->mTransformation;
	globalInverseTransform.Inverse();

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);

}

void Mesh::SetTransformForAllSubMeshes(NCLMatrix4 transform)
{
	for (SubMesh* submesh : meshes)
	{
		submesh->SetTransform(transform);
	}
}

void Mesh::ProcessNode(aiNode *node, const aiScene *scene)
{
	//Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		++meshCounter;
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		SubMesh* model = ProcessMesh(meshCounter, mesh, scene);
		meshes.push_back(model);
		meshesByName.insert({ static_cast<string>(mesh->mName.C_Str()), model });
	}

	//Then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

SubMesh* Mesh::ProcessMesh(unsigned int meshIndex, aiMesh* mesh, const aiScene *scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<Texture> heights;
	vector<VertexBoneData> bones;

	const float maxFloat = (std::numeric_limits<float>::max)();
	const float minFloat = (std::numeric_limits<float>::lowest());

	NCLVector3 minBounds(maxFloat, maxFloat, maxFloat);
	NCLVector3 maxBounds(minFloat, minFloat, minFloat);

	BoundingBox AABB;

	int baseVertex = 0;
	int numVertices = 0;

	for (int i = 0; i < meshIndex; ++i)
	{
		baseVertex = numVertices;
		numVertices += scene->mMeshes[i]->mNumVertices;
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//Process vertex positions, normals and texture coordinates
		NCLVector3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.Position = pos;

		//Check all points against both min and max to form AABB
		if (pos.x < minBounds.x) minBounds.x = pos.x;
		if (pos.y < minBounds.y) minBounds.y = pos.y;
		if (pos.z < minBounds.z) minBounds.z = pos.z;

		if (pos.x > maxBounds.x) maxBounds.x = pos.x;
		if (pos.y > maxBounds.y) maxBounds.y = pos.y;
		if (pos.z > maxBounds.z) maxBounds.z = pos.z;

		AABB.min = minBounds;
		AABB.max = maxBounds;

		NCLVector3 norm;
		norm.x = mesh->mNormals[i].x;
		norm.y = mesh->mNormals[i].y;
		norm.z = mesh->mNormals[i].z;
		vertex.Normal = norm;

		if (mesh->mTextureCoords[0]) //Does the mesh contain texture coordinates?
		{
			NCLVector2 tex;
			tex.x = mesh->mTextureCoords[0][i].x;
			tex.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = tex;
		}
		else vertex.TexCoords = NCLVector2(0.0f, 0.0f);

		//Tangent
		NCLVector3 tan;
		tan.x = mesh->mTangents[i].x;
		tan.y = mesh->mTangents[i].y;
		tan.z = mesh->mTangents[i].z;
		vertex.Tangent = tan;

		//Bitangent
		NCLVector3 bitan;
		bitan.x = mesh->mBitangents[i].x;
		bitan.y = mesh->mBitangents[i].y;
		bitan.z = mesh->mBitangents[i].z;
		vertex.Bitangent = bitan;

		vertices.push_back(vertex);
	}

	bones = LoadBones(meshIndex, mesh);

	//Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//Process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<Texture> heightMaps = LoadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_height");
		heights.insert(heights.end(), heightMaps.begin(), heightMaps.end());
	}

	SubMesh* modelMesh = new SubMesh(vertices, indices, textures, heights, AABB, numModels);

	if (textures.size() == 0)
	{
		modelMesh->hasTexture = 0;
		hasTexture = 0;
	}
	else
	{
		modelMesh->hasTexture = 1;
		hasTexture = 1;
	}

	modelMesh->baseVertex = baseVertex;

	return modelMesh;
}

Mesh * Mesh::GenerateHeightMap(int width, int height)
{
	SubMesh * heightMap = new SubMesh();

	for (int x = 0; x <= width; ++x)
	{
		for (int z = 0; z <= height; ++z)
		{
			//int indexOffset = (x * width) + z;

			Vertex vertex;
			float Xoffset = (float)(2.05 * ((x - width / 2)) / (float)width);
			float Zoffset = (float)(2.05 * ((z - height / 2)) / (float)height);

			vertex.Position = NCLVector3(Xoffset,5, Zoffset);
			vertex.Normal = NCLVector3(0, 1, 0);
			vertex.TexCoords = NCLVector2((float)x/width, (float)z/height);
			vertex.Tangent = NCLVector3(1, 0, 0);
			vertex.Bitangent = NCLVector3(0, 0, 1);
			//maybe need to generate tangents, bitangents?

			heightMap->vertices.push_back(vertex);
		}
	}


	for (int x = 0; x < width; ++x)
	{
		for (int z = 0; z < height; ++z)
		{
			int a = x * width + z;
			int b = (x + 1) * width + z;
			int c = (x + 1) * width + (z + 1);
			int d = (x * width) + (z + 1);

			heightMap->indices.push_back(c);
			heightMap->indices.push_back(b);
			heightMap->indices.push_back(a);

			heightMap->indices.push_back(a);
			heightMap->indices.push_back(d);
			heightMap->indices.push_back(c);

		}
	}
	heightMap->SetupMesh();
	Mesh* mesh = new Mesh();
	mesh->radius = 100;
	mesh->numModels = 1;
	mesh->meshes.push_back(heightMap);

	return mesh;
}

vector<Texture> Mesh::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		//If texture hasn't been loaded already, load it
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);

			loadedTextures.push_back(texture); //Add to loaded textures
		}
	}

	return textures;
}

unsigned int Mesh::TextureFromFile(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		stbi_image_free(data);
	}

	return textureID;
}

void Mesh::SetReflectionAttributesForAllSubMeshes(int isReflective, float strength)
{
	for (SubMesh* mesh : meshes)
	{
		mesh->isReflective = isReflective;
		mesh->reflectionStrength = strength;
	}
}

void Mesh::SetbackupColourAttributeForAllSubMeshes(NCLVector4 colour)
{
	for (SubMesh* mesh : meshes)
	{
		mesh->baseColour = colour;
	}
}

void Mesh::Draw(Shader& shader, NCLMatrix4 worldTransform)
{
	for (SubMesh* submesh : meshes)
	{
		submesh->Draw(shader, worldTransform);
	}
}

vector<VertexBoneData> Mesh::LoadBones(unsigned int meshIndex, const aiMesh * mesh)
{
	std::vector<VertexBoneData> bones;

	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIndex = 0;
		string boneName(mesh->mBones[i]->mName.data);

		if (boneMapping.find(boneName) == boneMapping.end())
		{
			// Allocate an index for a new bone
			boneIndex = numBones;
			numBones++;
			BoneInfo bi;
			boneInfo.push_back(bi);
			boneInfo[boneIndex].boneOffset = mesh->mBones[i]->mOffsetMatrix;
			boneMapping[boneName] = boneIndex;
		}
		else
		{
			boneIndex = boneMapping[boneName];
		}

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) 
		{
			unsigned int VertexID = meshes[meshIndex]->baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones[VertexID].AddBoneData(boneIndex, Weight);
		}
	}

	return bones;
}

void Mesh::loadTexture(std::string textureFile)
{
	//unsigned int texId = SOIL_load_OGL_texture(filepath.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);
	//for (SubMesh *subMesh : this->meshes) {
	//	subMesh->addTexture(texId);
	//}

	Texture texture;

	this->textureFile = textureFile;
	string filename = textureFile;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		stbi_image_free(data);
	}

	texture.id = textureID;
	texture.type = "texture_diffuse";
	texture.path = textureFile;

	for (SubMesh* submesh : meshes)
	{
		submesh->textures.push_back(texture);
		submesh->hasTexture = 1;
	}

	hasTexture = 1;
}
