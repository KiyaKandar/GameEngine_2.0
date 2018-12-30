#include "GameObjectBuilder.h"

GameObject* GameObjectBuilder::buildGameObject(Node* node, Database* database)
{
	SceneNode* sceneNode = buildSceneNode(node->children[0], database);
	GameObject* gameObject = new GameObject();
	
	gameObject->setSize(sizeof(GameObject));
	gameObject->setName(node->name);
	gameObject->setSceneNode(sceneNode);
	
	gameObject->stats.colourToPaint = VectorBuilder::buildVector4(node->children[0]->children[1]);
	gameObject->setScale(VectorBuilder::buildVector3(node->children[4]));

	if (node->children.size() >= 6)
	{
		PhysicsNode* physicsNode = buildPhysicsNode(node->children[5], gameObject);
		gameObject->setPhysicsNode(physicsNode);
	}

	gameObject->setPosition(VectorBuilder::buildVector3(node->children[2]));
	gameObject->setRotation(VectorBuilder::buildVector4(node->children[3]));

	return gameObject;
}

SceneNode* GameObjectBuilder::buildSceneNode(Node* node, Database* database)
{
	std::string meshName = node->children[0]->value;
	SceneNode* sceneNode = new SceneNode(static_cast<Mesh*>(database->getTable("Meshes")->getResource(meshName)));
	sceneNode->SetColour(VectorBuilder::buildVector4(node->children[1]));

	if (node->children.size() > 2)
	{
		sceneNode->isReflective = true;
		sceneNode->reflectiveStrength = stof(node->children[2]->value);
	}

	return sceneNode;
}

PhysicsNode* GameObjectBuilder::buildPhysicsNode(Node* node, GameObject* parent)
{
	PhysicsNode* physicsnode = new PhysicsNode();

	physicsnode->SetParent(parent);

	for (Node* child : node->children)
	{
		if (child->nodeType == "Enabled")
		{
			physicsnode->setEnabled(child->value == "True");
		}
		else if (child->nodeType == "TransmitCollision")
		{
			physicsnode->transmitCollision = child->value == "True";
		}
		else if (child->nodeType == "MultipleTransmitions")
		{
			physicsnode->multipleTransmitions = child->value == "True";
		}
		else if (child->nodeType == "CollisionShape")
		{
			physicsnode->setCollisionShape(child->value);
		}
		else if (child->nodeType == "Mass")
		{
			physicsnode->SetInverseMass(stof(child->value));
			physicsnode->SetInverseInertia(physicsnode->getCollisionShape()->BuildInverseInertia(physicsnode->GetInverseMass()));
		}
		else if (child->nodeType == "Elasticity")
		{
			physicsnode->SetElasticity(stof(child->value));
		}
		else if (child->nodeType == "Friction")
		{
			physicsnode->SetFriction(stof(child->value));
		}
		else if (child->nodeType == "Damping")
		{
			physicsnode->setDamping(stof(child->value));
		}
		else if (child->nodeType == "isStatic")
		{
			physicsnode->setStatic(child->value == "True");
		}
		else if (child->nodeType == "isCollision")
		{
			physicsnode->setIsCollision(child->value == "True");
		}
	}

	return physicsnode;
}
