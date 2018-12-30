#include "GameObject.h"

#include "../Physics/PhysicsNode.h"

GameObject::GameObject()
{
	setSize(sizeof(*this));
}

GameObject::~GameObject()
{
	if (sceneNode)
	{
		delete sceneNode;
	}

	if (physicsNode)
	{
		delete physicsNode;
	}
}

void GameObject::setSceneNode(SceneNode * sceneNode)
{
	this->sceneNode = sceneNode;
	sceneNode->setParent(this);
	sceneNode->takeLocalCopyOfMeshAnimations();
}

void GameObject::setPhysicsNode(PhysicsNode * physicsNode)
{
	this->physicsNode = physicsNode;
}

SceneNode * GameObject::getSceneNode()
{
	return sceneNode;
}

PhysicsNode * GameObject::getPhysicsNode()
{
	return physicsNode;
}

void GameObject::update(float dt)
{
	if (stats.executeAfter)
	{
		stats.timer += dt;
		if (stats.timer >= stats.timeToWait)
		{
			stats.executeAfter();
			stats.timer = 0.f;
		}
	}
	position = physicsNode->GetPosition();
	NCLMatrix4 newTransform = this->physicsNode->GetWorldSpaceTransform();
	newTransform = newTransform * NCLMatrix4::scale(scale);

	this->sceneNode->SetTransform(newTransform);
}

void GameObject::setPosition(NCLVector3 position)
{
	this->position = position;
	this->sceneNode->SetTransform(position);

	if(this->physicsNode != nullptr)
	{
		this->physicsNode->SetPosition(position);
	}
}

void GameObject::setRotation(NCLVector4 rotation)
{
	NCLVector3 position = sceneNode->GetTransform().getPositionVector();
	NCLVector3 scale = sceneNode->GetTransform().getScalingVector();
	sceneNode->axisAngleRotation = rotation;

	this->sceneNode->SetTransform(NCLMatrix4::translation(position) * 
		NCLMatrix4::rotation(rotation.w, NCLVector3(rotation.x, rotation.y, rotation.z)) * 
		NCLMatrix4::scale(scale));
	
	if (this->physicsNode != nullptr)
	{
		this->physicsNode->setRotation(rotation);
	}
}

void GameObject::setScale(NCLVector3 scale)
{
	this->scale = scale;
	this->sceneNode->SetModelScale(scale);
	if (physicsNode != nullptr)
	{
		this->physicsNode->getCollisionShape()->setScale(scale, this->physicsNode->GetInverseMass());
	}
	else
	{
		this->stats.defaultScale = scale;
	}
	
}

void GameObject::setEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
	if (this->physicsNode != nullptr)
	{
		this->physicsNode->setEnabled(isEnabled);
	}
	
	this->sceneNode->setEnabled(isEnabled);
}
