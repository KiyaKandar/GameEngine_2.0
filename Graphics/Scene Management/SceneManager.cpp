#include "SceneManager.h"

#include "../Utility/Camera.h"
#include "../Utility/Light.h"

SceneManager::SceneManager(Camera* camera, std::vector<SceneNode*>* sceneNodes)
{
	this->sceneNodes = sceneNodes;
	this->camera = camera;

	sceneNodes = new vector<SceneNode*>();
	sceneNodesInFrustum = new vector<SceneNode*>();
	transparentSceneNodesInFrustum = new vector<SceneNode*>();
	lights = new vector<Light*>();

	for (SceneNode* node : *sceneNodes)
	{
		if (node->isEnabled)
		sceneNodes->push_back(node);
	}
}

SceneManager::~SceneManager()
{
	delete sceneNodes;
	delete sceneNodesInFrustum;
	delete transparentSceneNodesInFrustum;
}

void SceneManager::clearMeshLists()
{
	sceneNodesInFrustum->clear();
	transparentSceneNodesInFrustum->clear();
}

void SceneManager::buildMeshLists()
{
	for (SceneNode* node : *sceneNodes)
	{
		node->Update(0.0f);
		allocateSubNodesToNodeLists(node);
	}
}

std::vector<SceneNode*>* SceneManager::getSceneNodesInFrustum()
{
	return sceneNodesInFrustum;
}

std::vector<SceneNode*>* SceneManager::getTransparentSceneNodesInFrustum()
{
	return transparentSceneNodesInFrustum;
}

std::vector<Light*>** SceneManager::getAllLights()
{
	return &lights;
}

std::vector<SceneNode*>** SceneManager::getAllNodes()
{
	return &sceneNodes;
}

//TODO ORDER MESHES
void SceneManager::allocateSubNodesToNodeLists(SceneNode* node)
{
	if (camera->sceneNodeIsInCameraView(node) && node->isEnabled)
	{
		node->GetMesh()->onScreen = true;

		if (node->getColour().w < 1.0f)
		{
			transparentSceneNodesInFrustum->push_back(node);
		}
		else
		{
			sceneNodesInFrustum->push_back(node);
		}
	}
	else
	{
		node->GetMesh()->onScreen = false;
	}
}
