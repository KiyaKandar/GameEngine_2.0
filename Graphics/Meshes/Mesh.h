#pragma once

#pragma comment(lib, "assimp-vc140-mt.lib")

#include "SOIL.h"
#include "SubMesh.h"
#include "../../Resource Management/Resources/Resource.h"

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include <iostream>
#include <string>
#include <vector>

#include <unordered_map>
#include <Simple OpenGL Image Library\src\stb_image_aug.h>
#include <map>

struct VertexBoneData
{
	unsigned int ids[NUM_BONES_PER_VEREX];
	float weights[NUM_BONES_PER_VEREX];

	void AddBoneData(unsigned int boneID, float weight)
	{
		int arraySize = sizeof(ids) / sizeof(ids[0]);

		for (unsigned int i = 0; i < arraySize; i++) 
		{
			if (weights[i] == 0.0) 
			{
				ids[i] = boneID;
				weights[i] = weight;
				return;
			}
		}
	}
};

struct BoneInfo
{
	aiMatrix4x4 boneOffset;
	aiMatrix4x4 finalTransformation;
};

class Mesh : public Resource
{
public:
	Mesh(char *path, int numModels)
	{
		this->numModels = numModels;
		LoadModel(path);

		file = path;

		setSize(sizeof(*this));
	}

	Mesh(const string path, int numModels)
	{
		this->numModels = numModels;
		LoadModel(path);

		file = path;
		
		setSize(sizeof(*this));
	}

	Mesh()
	{

	}

	~Mesh()
	{
		for (SubMesh* mesh : meshes)
		{
			delete mesh;
		}
	};

	void LoadModel(std::string path);

	void setupMesh()
	{
		for (SubMesh* submesh : meshes)
		{
			submesh->SetupMesh();
		}

		if (textureFile != "")
		{
			loadTexture(textureFile);
		}
	}
	void SetTransformForAllSubMeshes(NCLMatrix4 transform);

	void ProcessNode(aiNode *node, const aiScene *scene);
	SubMesh* ProcessMesh(unsigned int meshIndex, aiMesh *mesh, const aiScene *scene);


	static Mesh* GenerateHeightMap(int width, int height);

	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
		string typeName);

	static unsigned int TextureFromFile(const char *path, const string &directory);

	void SetReflectionAttributesForAllSubMeshes(int isReflective, float strength);
	void SetbackupColourAttributeForAllSubMeshes(NCLVector4 colour);

	virtual void Draw(Shader& shader,NCLMatrix4 worldTransform);

	float getRadius()
	{
		return this->meshes[0]->GetBoundingRadius();
	}
	
	std::vector<VertexBoneData> LoadBones(unsigned int meshIndex, const aiMesh* mesh);

	void loadTexture(std::string filepath);
	void setTextureFile(std::string textureFile)
	{
		this->textureFile = textureFile;
	}

	//Model Data 
	std::vector<SubMesh*> meshes;
	Mesh* mesh;
	unordered_map<string, SubMesh*> meshesByName;
	std::string directory;
	std::string file;
	std::string textureFile;
	std::vector<Texture> loadedTextures;

	Assimp::Importer import;
	const aiScene* scene;

	aiMatrix4x4 globalInverseTransform;
	map<string, unsigned  int> boneMapping; // maps a bone name to its index
	unsigned int numBones = 0;
	vector<BoneInfo> boneInfo;

	int hasTexture = 0;
	int numModels;

	float radius;

	unsigned int meshCounter = 0;
	bool perlin = false;
};

