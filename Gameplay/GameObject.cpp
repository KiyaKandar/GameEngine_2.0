#include "GameObject.h"

#include "../Physics/PhysicsNode.h"

GameObject::GameObject()
{
	SetSize(sizeof(*this));
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

void GameObject::SetSceneNode(SceneNode * sceneNode)
{
	this->sceneNode = sceneNode;
	sceneNode->SetParent(this);
	sceneNode->TakeLocalCopyOfMeshAnimations();
}

void GameObject::SetPhysicsNode(PhysicsNode * physicsNode)
{
	this->physicsNode = physicsNode;
}

SceneNode * GameObject::GetSceneNode()
{
	return sceneNode;
}

PhysicsNode * GameObject::GetPhysicsNode()
{
	return physicsNode;
}

void GameObject::Update(float dt)
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

void GameObject::SetPosition(NCLVector3 position)
{
	this->position = position;
	this->sceneNode->SetTransform(position);

	if(this->physicsNode != nullptr)
	{
		this->physicsNode->SetPosition(position);
	}
}

void GameObject::SetRotation(NCLVector4 rotation)
{
	NCLVector3 position = sceneNode->GetTransform().getPositionVector();
	NCLVector3 scale = sceneNode->GetTransform().getScalingVector();
	sceneNode->axisAngleRotation = rotation;

	this->sceneNode->SetTransform(NCLMatrix4::translation(position) * 
		NCLMatrix4::rotation(rotation.w, NCLVector3(rotation.x, rotation.y, rotation.z)) * 
		NCLMatrix4::scale(scale));
	
	if (this->physicsNode != nullptr)
	{
		this->physicsNode->SetRotation(rotation);
	}
}

void GameObject::SetScale(NCLVector3 scale)
{
	this->scale = scale;
	this->sceneNode->SetModelScale(scale);
	if (physicsNode != nullptr)
	{
		this->physicsNode->GetCollisionShape()->SetScale(scale, this->physicsNode->GetInverseMass());
	}
	else
	{
		this->stats.defaultScale = scale;
	}
	
}

void GameObject::SetEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
	if (this->physicsNode != nullptr)
	{
		this->physicsNode->SetEnabled(isEnabled);
	}
	
	this->sceneNode->SetEnabled(isEnabled);
}
