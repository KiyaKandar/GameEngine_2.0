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

void Level::loadLevelFile(std::string levelFilePath, GameplaySystem* gameplay)
{
	gameplay->deleteGameObjectScripts();
	parser.loadXMLFile(levelFilePath);
	levelNode = *parser.parsedXml;

	for (Node* child : levelNode.children)
	{
		loadLevelNode(child, gameplay);
	}

	addObjectsToGame();
}

void Level::loadLevelNode(Node* resourceNode, GameplaySystem* gameplay)
{
	if (resourceNode->nodeType == "UI")
	{
		loadUINode(resourceNode);
	}
	else if (resourceNode->nodeType == "GamePlay")
	{
		loadGameplayScripts(resourceNode, gameplay);
	}
	else if (resourceNode->nodeType == "GameLogic")
	{
		loadgameLogicScripts(resourceNode, gameplay);
	}
	else
	{
		loadResource(resourceNode);
	}
}

void Level::unloadLevelWhileKeepingUserInterface()
{
	MoveCameraRelativeToGameObjectMessage::resourceName = "";
	(*sceneManager->getAllNodes())->clear();
	(*sceneManager->getAllLights())->clear();
	physics->RemoveAllPhysicsObjects();

	std::vector<Table<Resource>*> tables = database->getAllTables();

	for (Table<Resource>* table : tables)
	{
		if (table->getName() != "UIMeshes")
		{
			table->getAllResources()->deleteAllResources();
		}
	}
}

void Level::unloadLevel() const
{
	MoveCameraRelativeToGameObjectMessage::resourceName = "";
	(*sceneManager->getAllNodes())->clear();
	(*sceneManager->getAllLights())->clear();
	physics->RemoveAllPhysicsObjects();

	std::vector<Table<Resource>*> tables = database->getAllTables();

	for (Table<Resource>* table : tables)
	{
		table->getAllResources()->deleteAllResources();
	}
}

void Level::addObjectsToGame() const
{
	auto gameObjectResources = database->getTable("GameObjects")->getAllResources()->getResourceBuffer();
	for (auto gameObjectIterator = gameObjectResources.begin(); gameObjectIterator != gameObjectResources.end(); gameObjectIterator++)
	{
		(*sceneManager->getAllNodes())->push_back(static_cast<GameObject*>((*gameObjectIterator).second)->getSceneNode());
		PhysicsNode* pnode = static_cast<GameObject*>((*gameObjectIterator).second)->getPhysicsNode();
		if (pnode != nullptr)
			physics->addPhysicsObject(pnode);
	}

	auto lightsResources = database->getTable("Lights")->getAllResources()->getResourceBuffer();
	for (auto lightsIterator = lightsResources.begin(); lightsIterator != lightsResources.end(); lightsIterator++)
	{
		(*sceneManager->getAllLights())->push_back(static_cast<Light*>((*lightsIterator).second));
	}
}

void Level::loadUINode(Node* resourceNode) const
{
	userInterface->setMenuFile(resourceNode->children[0]->value);
}

void Level::loadGameplayScripts(Node* resourceNode, GameplaySystem* gameplay) const
{
	gameplay->compileGameplayScript(LEVELDIR + resourceNode->children[0]->value);
}

void Level::loadgameLogicScripts(Node* resourceNode, GameplaySystem* gameplay) const
{
	for (Node* grandChild : resourceNode->children)
	{
		gameplay->addGameObjectScript(LEVELDIR + grandChild->value);
	}
}

void Level::loadResource(Node* resourceNode)
{
	parser.loadXMLFile(LEVELDIR + resourceNode->value);

	for (Node* grandchild : parser.parsedXml->children)
	{
		database->addResourceToTable(grandchild->nodeType, grandchild);
	}
}
