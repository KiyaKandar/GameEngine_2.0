#pragma once
#include "../Graphics/Scene Management/SceneNode.h"
#include "../Resource Management/Resources/Resource.h"
#include "../Utilities/Maths/Vector3.h"
#include <functional>

class PhysicsNode;

struct PaintGameStats
{
	NCLVector4 colourToPaint;
	NCLVector3 defaultScale;

	int maxPaint;
	int currentPaint = 0;
	int meteors;

	float defaultInvMass = 1.f;
	float timeToWait;
	float timer;

	bool canJump = false;

	std::function<void()> executeAfter = std::function<void()>();
};

class GameObject : public Resource
{
public:
	GameObject();
	~GameObject();

	void SetSceneNode(SceneNode* sceneNode);
	void SetPhysicsNode(PhysicsNode* physicsNode);

	SceneNode* GetSceneNode();
	PhysicsNode* GetPhysicsNode();

	void Update(float dt);
	void SetPosition(NCLVector3 position);
	void SetRotation(NCLVector4 rotation);
	void SetScale(NCLVector3 scale);
	void SetEnabled(bool isEnabled);

	 const NCLVector3& GetScale() const 
	{
		return scale;
	}

	const NCLVector3& GetPosition() const
	{
		return position;
	}

	bool GetEnabled() const
	{
		return isEnabled;
	}

	bool isEnabled = true;
	PaintGameStats stats;

private: 
	SceneNode *sceneNode = nullptr;
	PhysicsNode *physicsNode = nullptr;

	NCLVector3 position;
	NCLVector3 scale;

};

