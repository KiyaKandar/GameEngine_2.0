#pragma once

#pragma comment(lib, "assimp-vc140-mt.lib")

#include "SOIL.h"
#include "SubMesh.h"
#include "../Animation/Animation.h"
#include "../Animation/AnimationPlayer.h"
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

class Mesh : public Resource
{
public:
	Mesh(char *path, int numModels)
	{
		meshScene = nullptr;
		this->numModels = numModels;
		LoadModel(path);

		file = path;

		setSize(sizeof(*this));
	}

	Mesh(const string path, int numModels, const string name = "")
	{
		meshScene = nullptr;
		setName(name);

		this->numModels = numModels;
		LoadModel(path);

		file = path;
		
		setSize(sizeof(*this));
	}

	Mesh()
	{
		meshScene = nullptr;
	}

	~Mesh()
	{
		for (SubMesh* mesh : meshes)
		{
			delete mesh;
		}
	};

	void LoadModel(std::string path);
	void LoadMesh(std::string path);
	void LoadMD5ProxyFile(std::string path);

	void setupMesh()
	{
		for (SubMesh* submesh : meshes)
		{
			submesh->SetupMesh();

			for (Texture& texture : submesh->textures)
			{
				texture.id = TextureFromFile(texture.path.c_str(), directory);
			}
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

	vector<Texture> PrepareMaterialTextureLoading(aiMaterial *mat, aiTextureType type, string typeName);

	static unsigned int TextureFromFile(const char *path, const string &directory);

	void SetReflectionAttributesForAllSubMeshes(int isReflective, float strength);
	void SetbackupColourAttributeForAllSubMeshes(NCLVector4 colour);

	virtual void Draw(Shader& shader,NCLMatrix4 worldTransform);

	float getRadius()
	{
		return this->meshes[0]->GetBoundingRadius();
	}
	
	vector<VertexBoneData> LoadBones(const aiMesh* mesh, vector<BoneInfo>& boneInfo);

	void loadTexture(std::string filepath);
	void setTextureFile(std::string textureFile)
	{
		this->textureFile = textureFile;
	}

	bool onScreen = false;

	//Model Data 
	vector<BoneInfo> boneInfo;
	std::vector<SubMesh*> meshes;
	Mesh* mesh;
	unordered_map<string, SubMesh*> meshesByName;
	std::string directory;
	std::string file;
	std::string textureFile;
	std::vector<Texture> loadedTextures;

	Assimp::Importer import;
	Assimp::Importer animationImporters[10];
	int lastUsedAnimationImporter = 0;
	std::vector<const aiScene*> importedAnimations;
	bool hasAnimations = false;
	const aiScene* meshScene;

	aiMatrix4x4 globalInverseTransform;
	map<string, unsigned  int> boneMapping; // maps a bone name to its index
	unsigned int numBones = 0;

	int hasTexture = 0;
	int numModels;

	float radius;
	unsigned int meshCounter = 0;
	bool perlin = false;
};

