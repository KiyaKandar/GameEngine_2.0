#pragma once
#include "../Graphics/Scene Management/SceneNode.h"
#include "../Gameplay/GameObject.h"
#include "../Physics/PhysicsNode.h"
#include "Database\Database.h"
#include "XMLParser.h"
#include <unordered_map>
#include <functional>


class GameObjectBuilder
{
public:
	static GameObject* BuildGameObject(Node* node, Database* database);
	static SceneNode* BuildSceneNode(Node* node, Database* database);
	static PhysicsNode* BuildPhysicsNode(Node* node, GameObject* parent);
};

