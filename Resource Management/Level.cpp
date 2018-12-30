#include "Level.h"

#include <fstream>
#include <stdexcept>

#include "../Graphics/Utility/Light.h"
#include "../Interface/UserInterface.h"
#include "../Gameplay/GameplaySystem.h"

Level::Level(Database *database, SceneManager* sceneManager,
	PhysicsEngine* physics, UserInterface* userInterface)
{
	this->database = database;
	this->sceneManager = sceneManager;
	this->physics = physics;
	this->userInterface = userInterface;
}

Level::~Level()
{
}

void Level::LoadLevelFile(std::string levelFilePath, GameplaySystem* gameplay)
{
	gameplay->DeleteGameObjectScripts();
	parser.LoadXmlFile(levelFilePath);
	levelNode = *parser.parsedXml;

	for (Node* child : levelNode.children)
	{
		LoadLevelNode(child, gameplay);
	}

	AddObjectsToGame();
}

void Level::LoadLevelNode(Node* resourceNode, GameplaySystem* gameplay)
{
	if (resourceNode->nodeType == "UI")
	{
		LoadUiNode(resourceNode);
	}
	else if (resourceNode->nodeType == "GamePlay")
	{
		LoadGameplayScripts(resourceNode, gameplay);
	}
	else if (resourceNode->nodeType == "GameLogic")
	{
		LoadgameLogicScripts(resourceNode, gameplay);
	}
	else
	{
		LoadResource(resourceNode);
	}
}

void Level::UnloadLevelWhileKeepingUserInterface()
{
	MoveCameraRelativeToGameObjectMessage::resourceName = "";
	(*sceneManager->GetAllNodes())->clear();
	(*sceneManager->GetAllLights())->clear();
	physics->RemoveAllPhysicsObjects();

	std::vector<Table<Resource>*> tables = database->GetAllTables();

	for (Table<Resource>* table : tables)
	{
		if (table->GetName() != "UIMeshes")
		{
			table->GetAllResources()->DeleteAllResources();
		}
	}
}

void Level::UnloadLevel() const
{
	MoveCameraRelativeToGameObjectMessage::resourceName = "";
	(*sceneManager->GetAllNodes())->clear();
	(*sceneManager->GetAllLights())->clear();
	physics->RemoveAllPhysicsObjects();

	std::vector<Table<Resource>*> tables = database->GetAllTables();

	for (Table<Resource>* table : tables)
	{
		table->GetAllResources()->DeleteAllResources();
	}
}

void Level::AddObjectsToGame() const
{
	auto gameObjectResources = database->GetTable("GameObjects")->GetAllResources()->GetResourceBuffer();
	for (auto gameObjectIterator = gameObjectResources.begin(); gameObjectIterator != gameObjectResources.end(); gameObjectIterator++)
	{
		(*sceneManager->GetAllNodes())->push_back(static_cast<GameObject*>((*gameObjectIterator).second)->GetSceneNode());
		PhysicsNode* pnode = static_cast<GameObject*>((*gameObjectIterator).second)->GetPhysicsNode();
		if (pnode != nullptr)
			physics->AddPhysicsObject(pnode);
	}

	auto lightsResources = database->GetTable("Lights")->GetAllResources()->GetResourceBuffer();
	for (auto lightsIterator = lightsResources.begin(); lightsIterator != lightsResources.end(); lightsIterator++)
	{
		(*sceneManager->GetAllLights())->push_back(static_cast<Light*>((*lightsIterator).second));
	}
}

void Level::LoadUiNode(Node* resourceNode) const
{
	userInterface->SetMenuFile(resourceNode->children[0]->value);
}

void Level::LoadGameplayScripts(Node* resourceNode, GameplaySystem* gameplay) const
{
	gameplay->CompileGameplayScript(LEVELDIR + resourceNode->children[0]->value);
}

void Level::LoadgameLogicScripts(Node* resourceNode, GameplaySystem* gameplay) const
{
	for (Node* grandChild : resourceNode->children)
	{
		gameplay->AddGameObjectScript(LEVELDIR + grandChild->value);
	}
}

void Level::LoadResource(Node* resourceNode)
{
	parser.LoadXmlFile(LEVELDIR + resourceNode->value);

	for (Node* grandchild : parser.parsedXml->children)
	{
		database->AddResourceToTable(grandchild->nodeType, grandchild);
	}
}
