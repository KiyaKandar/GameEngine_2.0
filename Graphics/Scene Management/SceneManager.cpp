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

void SceneManager::ClearMeshLists()
{
	sceneNodesInFrustum->clear();
	transparentSceneNodesInFrustum->clear();
}

void SceneManager::BuildMeshLists()
{
	for (SceneNode* node : *sceneNodes)
	{
		node->Update(0.0f);
		AllocateSubNodesToNodeLists(node);
	}
}

std::vector<SceneNode*>* SceneManager::GetSceneNodesInFrustum()
{
	return sceneNodesInFrustum;
}

std::vector<SceneNode*>* SceneManager::GetTransparentSceneNodesInFrustum()
{
	return transparentSceneNodesInFrustum;
}

std::vector<Light*>** SceneManager::GetAllLights()
{
	return &lights;
}

std::vector<SceneNode*>** SceneManager::GetAllNodes()
{
	return &sceneNodes;
}

//TODO ORDER MESHES
void SceneManager::AllocateSubNodesToNodeLists(SceneNode* node)
{
	if (camera->SceneNodeIsInCameraView(node) && node->isEnabled)
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
