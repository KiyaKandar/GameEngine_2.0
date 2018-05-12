#pragma once

#include <functional>

#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Matrix4.h"
#include "../Utilities/Maths/Matrix3.h"
#include "../Utilities/Maths/Quaternion.h"
#include "../Gameplay/GameObject.h"

#include "CollisionShape.h"
#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"

#include "CollisionDetectionSAT.h"
#include "../Launch/Networking/DeadReckoning.h"

#include <cmath>

class PhysicsNode;

typedef std::function<bool(PhysicsNode* this_obj, PhysicsNode* colliding_obj, CollisionData)> PhysicsCollisionCallback;
typedef std::function<void(const NCLMatrix4& transform)> PhysicsUpdateCallback;

//class GameObject;

class PhysicsNode
{
public:
	PhysicsNode()
		: position(0.0f, 0.0f, 0.0f)
		, linVelocity(0.0f, 0.0f, 0.0f)
		, force(0.0f, 0.0f, 0.0f)
		, invMass(0.0f)
		, orientation(0.0f, 0.0f, 0.0f, 1.0f)
		, angVelocity(0.0f, 0.0f, 0.0f)
		, torque(0.0f, 0.0f, 0.0f)
		, invInertia(NCLMatrix3::ZeroMatrix)
		, collisionShape(NULL)
		, friction(0.5f)
		, damping(0.99999999f)
		, elasticity(0.9f)
		, enabled(true)
		, isCollision(true)
		, isStatic(true)
		, appliedForce(0.0f, 0.0f, 0.0f)
		, acceleration(0.0f, 0.0f, 0.0f)
	{
		
	}
	~PhysicsNode()
	{
		delete collisionShape;
		collisionShape = nullptr;
	};

	void integrateForVelocity(float dt);
	void integrateForPosition(float dt);

	inline GameObject* getParent() const 
	{ 
		return parent; 
	}
	inline float getElasticity() const 
	{ 
		return elasticity; 
	}
	inline float getFriction() const 
	{ 
		return friction; 
	}
	inline const NCLVector3& getPosition() const 
	{ 
		return position; 
	}
	inline const NCLVector3& getLinearVelocity() const 
	{ 
		return linVelocity; 
	}
	inline const NCLVector3& getForce() const 
	{ 
		return force; 
	}
	inline float getInverseMass() const 
	{ 
		return invMass; 
	}
	inline const Quaternion& getOrientation() const 
	{ 
		return orientation; 
	}
	inline const NCLVector3& getAngularVelocity() const 
	{ 
		return angVelocity; 
	}
	inline const NCLVector3& getTorque() const 
	{ 
		return torque; 
	}
	inline const NCLMatrix3& getInverseInertia() const 
	{ 
		return invInertia; 
	}
	inline CollisionShape* getCollisionShape() const 
	{ 
		return collisionShape; 
	}
	const NCLMatrix4& getWorldSpaceTransform() const 
	{ 
		return worldTransform; 
	}
	inline const bool getEnabled() const
	{
		return enabled;
	}
	inline const bool getIsCollision() const
	{
		return isCollision;
	}

	inline const bool getIsStatic() const
	{
		return isStatic;
	}

	void setRotation(NCLVector4 rotation)
	{
		worldTransform = (NCLMatrix4::translation(position) *
			NCLMatrix4::rotation(rotation.w, NCLVector3(rotation.x, rotation.y, rotation.z)) *
			NCLMatrix4::scale(worldTransform.getScalingVector()));

		orientation = Quaternion::axisAngleToQuaterion(NCLVector3(rotation.x, rotation.y, rotation.z), rotation.w);
	}

	NCLVector3 getAcceleration()
	{
		return acceleration;
	}

	void setAcceleration(NCLVector3 newAcceleration)
	{
		acceleration = newAcceleration;
	}

	inline void setParent(GameObject* obj) 
	{ 
		parent = obj; 
	}
	inline void setElasticity(float elasticityCoeff) 
	{ 
		elasticity = elasticityCoeff; 
	}
	inline void setFriction(float frictionCoeff) 
	{ 
		friction = frictionCoeff; 
	}

	inline void setDamping(float dampingCoeff)
	{
		damping = dampingCoeff;
	}

	float getDamping()
	{
		return damping;
	}

	inline void setPosition(const NCLVector3& v) 
	{ 
		position = v; 
		
		worldTransform = orientation.toMatrix();

		worldTransform.setPositionVector(position);
	}
	inline void setLinearVelocity(const NCLVector3& v) 
	{ 
		linVelocity = v; 
	}

	inline void setForce(const NCLVector3& v)
	{
		force = v;
	}
	
	inline void setInverseMass(const float& v) 
	{ 
		invMass = v; 
	}

	inline void setOrientation(const Quaternion& v) 
	{ 
		orientation = v; 
		
		worldTransform = orientation.toMatrix();

		worldTransform.setPositionVector(position);
	}
	inline void setAngularVelocity(const NCLVector3& v) 
	{ 
		angVelocity = v; 
	}
	inline void setTorque(const NCLVector3& v) 
	{ 
		torque = v; 
	}
	inline void setInverseInertia(const NCLMatrix3& v) 
	{ 
		invInertia = v; 
	}

	inline void setCollisionShape(std::string colshape)
	{
		CollisionShape* colShape;
		
		if (colshape == "Sphere")
		{
			colShape = new SphereCollisionShape(parent->getScale().x);
			collisionShapeType = "Sphere";
		} 
		else if (colshape == "Box")
		{
			colShape = new CuboidCollisionShape(parent->getScale());
			collisionShapeType = "Box";
		}
		else
		{
			colShape = nullptr;
		}
		
		if (collisionShape) collisionShape->setParent(NULL);
		collisionShape = colShape;
		if (collisionShape) collisionShape->setParent(this);

		setBroadPhaseShape("Sphere");
	}


	inline void setBroadPhaseShape(std::string broadphaseShapeString)
	{
		CollisionShape* colShape;
		
		if (broadphaseShapeString == "Sphere")
		{
			colShape = new SphereCollisionShape(sqrt(((pow(parent->getScale().x,2)))  + ((pow(parent->getScale().y, 2))) + ((pow(parent->getScale().z, 2)))));		
		}
		else
		{
			colShape = nullptr;
		}

		if (collisionShape) collisionShape->setParent(NULL);
		this->broadPhaseShape = colShape;
		if (collisionShape) collisionShape->setParent(this);
	}


	inline CollisionShape* getBroadPhaseShape() const
	{
		return broadPhaseShape;
	}


	inline void setEnabled(bool isPhy)
	{
		enabled = isPhy;
	}
	inline void setIsCollision(bool isCol)
	{
		isCollision = isCol;
	}

	inline void setStatic(bool isStat)
	{
		isStatic = isStat;
	}

	inline void setOnCollisionCallback(PhysicsCollisionCallback callback) 
	{ 
		onCollisionCallback = callback; 
	}
	inline bool fireOnCollisionEvent(PhysicsNode* obj_a, PhysicsNode* obj_b, CollisionData collisionData)
	{
		return (onCollisionCallback) ? onCollisionCallback(obj_a, obj_b, collisionData) : true;
	}

	inline void setOnUpdateCallback(PhysicsUpdateCallback callback)
	{
		onUpdateCallbacks.push_back(callback);
	}

	inline void fireOnUpdateCallback()
	{
		if (worldTransform.getPositionVector() != previousTransform.getPositionVector())
		{
			for (PhysicsUpdateCallback callback : onUpdateCallbacks)
			{
				if (callback) callback(worldTransform);
			}

			movedSinceLastBroadPhase = true;
		}

		previousTransform = worldTransform;
	}

	bool toDeleteInOctree = false;
	bool movedSinceLastBroadPhase = false;
	bool transmitCollision = false;
	bool multipleTransmitions = false;
	bool hasTransmittedCollision = false;


	inline void setAppliedForce(NCLVector3 appliedForce)
	{
		this->appliedForce += appliedForce;
	}

	inline void applyImpulse(NCLVector3 impulse)
	{
		linVelocity += impulse;
	}
	
	bool constantAcceleration = false;

	NCLVector3 startPosition;
	NCLVector3 startVelocity;
	NCLVector3 startAcceleration;

	std::string collisionShapeType;

private:
	GameObject*				parent;
	NCLMatrix4					worldTransform;
	NCLMatrix4 previousTransform;
	PhysicsUpdateCallback	onUpdateCallback;
	std::vector<PhysicsUpdateCallback>	onUpdateCallbacks;


	NCLVector3		position;
	NCLVector3		linVelocity;
	NCLVector3		force;
	NCLVector3		acceleration;
	float		invMass;

	Quaternion  orientation;
	NCLVector3		angVelocity;
	NCLVector3		torque;
	NCLMatrix3     invInertia;

	CollisionShape*		collisionShape;
	CollisionShape*		broadPhaseShape;
	PhysicsCollisionCallback	onCollisionCallback;

	float	elasticity;
	float	friction;
	float	damping;

	bool enabled;
	bool isCollision;
	bool isStatic;

	NCLVector3 appliedForce;
};

