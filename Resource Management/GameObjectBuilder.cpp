#include "GameObjectBuilder.h"

GameObject* GameObjectBuilder::BuildGameObject(Node* node, Database* database)
{
	SceneNode* sceneNode = BuildSceneNode(node->children[0], database);
	GameObject* gameObject = new GameObject();
	
	gameObject->SetSize(sizeof(GameObject));
	gameObject->SetName(node->name);
	gameObject->SetSceneNode(sceneNode);
	
	gameObject->stats.colourToPaint = VectorBuilder::buildVector4(node->children[0]->children[1]);
	gameObject->SetScale(VectorBuilder::buildVector3(node->children[4]));

	if (node->children.size() >= 6)
	{
		PhysicsNode* physicsNode = BuildPhysicsNode(node->children[5], gameObject);
		gameObject->SetPhysicsNode(physicsNode);
	}

	gameObject->SetPosition(VectorBuilder::buildVector3(node->children[2]));
	gameObject->SetRotation(VectorBuilder::buildVector4(node->children[3]));

	return gameObject;
}

SceneNode* GameObjectBuilder::BuildSceneNode(Node* node, Database* database)
{
	std::string meshName = node->children[0]->value;
	SceneNode* sceneNode = new SceneNode(static_cast<Mesh*>(database->GetTable("Meshes")->GetResource(meshName)));
	sceneNode->SetColour(VectorBuilder::buildVector4(node->children[1]));

	if (node->children.size() > 2)
	{
		sceneNode->isReflective = true;
		sceneNode->reflectiveStrength = stof(node->children[2]->value);
	}

	return sceneNode;
}

PhysicsNode* GameObjectBuilder::BuildPhysicsNode(Node* node, GameObject* parent)
{
	PhysicsNode* physicsnode = new PhysicsNode();

	physicsnode->SetParent(parent);

	for (Node* child : node->children)
	{
		if (child->nodeType == "Enabled")
		{
			physicsnode->SetEnabled(child->value == "True");
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
			physicsnode->SetCollisionShape(child->value);
		}
		else if (child->nodeType == "Mass")
		{
			physicsnode->SetInverseMass(stof(child->value));
			physicsnode->SetInverseInertia(physicsnode->GetCollisionShape()->BuildInverseInertia(physicsnode->GetInverseMass()));
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
			physicsnode->SetDamping(stof(child->value));
		}
		else if (child->nodeType == "isStatic")
		{
			physicsnode->SetStatic(child->value == "True");
		}
		else if (child->nodeType == "isCollision")
		{
			physicsnode->SetIsCollision(child->value == "True");
		}
	}

	return physicsnode;
}
