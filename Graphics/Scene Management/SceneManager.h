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

	void clearMeshLists();
	void buildMeshLists();

	std::vector<SceneNode*>* getSceneNodesInFrustum();
	std::vector<SceneNode*>* getTransparentSceneNodesInFrustum();

	std::vector<Light*>** getAllLights();
	std::vector<SceneNode*>** getAllNodes();

private:
	void allocateSubNodesToNodeLists(SceneNode* node);

	Camera* camera;

	std::vector<Light*>* lights;
	std::vector<SceneNode*>* sceneNodes;
	std::vector<SceneNode*>* sceneNodesInFrustum;
	std::vector<SceneNode*>* transparentSceneNodesInFrustum;

};

