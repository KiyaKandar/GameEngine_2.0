#pragma once

#include "XMLParser.h"
#include "Database\Database.h"
#include "Database\TableCreation.h"
#include "../Graphics/Scene Management/SceneManager.h"
#include "../Gameplay/GameObject.h"
#include "../Physics/PhysicsEngine.h"
#include <string>
#include <vector>
#include <set>
#include "../Utilities/FilePaths.h"


class UserInterface;
class GameplaySystem;

class Level
{
public:
	Level(Database* database, SceneManager* sceneManager,
		PhysicsEngine* physics, UserInterface* userInterface);
	~Level();

	void loadLevelFile(std::string levelFilePath, GameplaySystem* gameplay);
	void loadLevelNode(Node* resourceNode, GameplaySystem* gameplay);

	void unloadLevelWhileKeepingUserInterface();
	void unloadLevel() const;

	void addObjectsToGame() const;

private:
	void loadUINode(Node* resourceNode) const;
	void loadResource(Node* resourceNode);

	void loadGameplayScripts(Node* resourceNode, GameplaySystem* gameplay) const;
	void loadgameLogicScripts(Node* resourceNode, GameplaySystem* gameplay) const;

	XMLParser parser;
	Database *database;
	SceneManager* sceneManager;
	PhysicsEngine* physics;
	UserInterface* userInterface;
	Node levelNode;
};