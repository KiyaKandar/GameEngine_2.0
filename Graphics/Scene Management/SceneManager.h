#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")
#include "SceneNode.h"
#include "../Meshes/SubMesh.h"

#include <vector>

class Camera;
class Light;

class SceneManager
{
public:
	explicit SceneManager(Camera* camera, std::vector<SceneNode*>* sceneNodes);
	~SceneManager();

	void ClearMeshLists();
	void BuildMeshLists();

	std::vector<SceneNode*>* GetSceneNodesInFrustum();
	std::vector<SceneNode*>* GetTransparentSceneNodesInFrustum();

	std::vector<Light*>** GetAllLights();
	std::vector<SceneNode*>** GetAllNodes();

private:
	void AllocateSubNodesToNodeLists(SceneNode* node);

	Camera* camera;

	std::vector<Light*>* lights;
	std::vector<SceneNode*>* sceneNodes;
	std::vector<SceneNode*>* sceneNodesInFrustum;
	std::vector<SceneNode*>* transparentSceneNodesInFrustum;

};

