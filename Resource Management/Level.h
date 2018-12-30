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

	void LoadLevelFile(std::string levelFilePath, GameplaySystem* gameplay);
	void LoadLevelNode(Node* resourceNode, GameplaySystem* gameplay);

	void UnloadLevelWhileKeepingUserInterface();
	void UnloadLevel() const;

	void AddObjectsToGame() const;

private:
	void LoadUiNode(Node* resourceNode) const;
	void LoadResource(Node* resourceNode);

	void LoadGameplayScripts(Node* resourceNode, GameplaySystem* gameplay) const;
	void LoadgameLogicScripts(Node* resourceNode, GameplaySystem* gameplay) const;

	XMLParser parser;
	Database *database;
	SceneManager* sceneManager;
	PhysicsEngine* physics;
	UserInterface* userInterface;
	Node levelNode;
};