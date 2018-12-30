#pragma once

#include "../Utilities/Maths/Quaternion.h"
#include "../Utilities/Maths/Matrix3.h"
#include "CollisionShape.h"
#include <functional>
#include "../Gameplay/GameObject.h"

#include "CollisionDetectionSAT.h"
#include "../Launch/Networking/DeadReckoning.h"

#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"

#include "../Communication/MessageSenders/TrackedMessageSender.h"
#include "../Communication/Messages/CollisionMessage.h"

class PhysicsNode;

//Callback function called whenever a collision is detected between two objects
//Params:
//	PhysicsNode* this_obj		- The current object class that contains the callback
//	PhysicsNode* colliding_obj	- The object that is colliding with the given object
//Return:
//  True	- The physics engine should process the collision as normal
//	False	- The physics engine should drop the collision pair and not do any further collision resolution/manifold generation
//			  > This can be useful for AI to see if a player/agent is inside an area/collision volume
typedef std::function<bool(PhysicsNode* this_obj, PhysicsNode* colliding_obj, CollisionData)> PhysicsCollisionCallback;

//Callback function called whenever this physicsnode's world transform is updated
typedef std::function<void(const NCLMatrix4& transform)> PhysicsUpdateCallback;

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
		, friction(0.9f)
		, elasticity(0.1f)
		, damping(0.99f)
		, enabled(true)
		, isCollision(true)
		, isStatic(true)
		, appliedForce(0.0f, 0.0f, 0.0f)
	{
	}

	virtual ~PhysicsNode()
	{
		for each (CollisionShape* shape in collisionShapes)
		{
			delete shape;
			shape = nullptr;
		}
	}

	inline void SetCollisionShape(std::string colshape)
	{
		CollisionShape* colShape;

		if (colshape == "Sphere")
		{
			colShape = new SphereCollisionShape(parent->GetScale().x);
			colShape->SetParent(this);
			collisionShapeType = "Sphere";
		}
		else if (colshape == "Box")
		{
			colShape = new CuboidCollisionShape(parent->GetScale());
			colShape->SetParent(this);
			collisionShapeType = "Box";
		}
		else
		{
			colShape = nullptr;
		}

		collisionShapes.clear();
		AddCollisionShape(colShape);
	}

	//<-------- Integration --------->
	// Called automatically by PhysicsEngine on all physics nodes each frame
	void IntegrateForVelocity(float dt);
	//<-- Between calling these two functions the physics engine will solve velocity to get 'true' final velocity -->
	void IntegrateForPosition(float dt);


	//<--------- GETTERS ------------->
	inline GameObject*			GetParent()					const { return parent; }

	inline float				GetElasticity()				const { return elasticity; }
	inline float				GetFriction()				const { return friction; }

	inline const NCLVector3&		GetPosition()				const { return position; }
	inline const NCLVector3&		GetLinearVelocity()			const { return linVelocity; }
	inline const NCLVector3&		GetForce()					const { return force; }
	inline const NCLVector3&		GetAcceleration()					const { return acceleration; }
	inline float				GetInverseMass()			const { return invMass; }

	inline const Quaternion&	GetOrientation()			const { return orientation; }
	inline const NCLVector3&		GetAngularVelocity()		const { return angVelocity; }
	inline const NCLVector3&		GetTorque()					const { return torque; }
	inline const NCLMatrix3&		GetInverseInertia()			const { return invInertia; }

	inline const bool GetIsCollision() const
	{
		return isCollision;
	}

	float GetDamping() const
	{
		return damping;
	}

	inline CollisionShape* GetCollisionShape() const
	{
		return collisionShapes[0];
	}

	inline void SetEnabled(bool isPhy)
	{
		enabled = isPhy;
	}

	inline const bool GetEnabled() const
	{
		return enabled;
	}

	inline void SetIsCollision(bool isCol)
	{
		isCollision = isCol;
	}

	inline void SetDamping(float dampingCoeff)
	{
		damping = dampingCoeff;
	}

	inline const bool GetIsStatic() const
	{
		return isStatic;
	}

	inline void SetStatic(bool isStat)
	{
		isStatic = isStat;
	}

	inline NCLVector3 GetAppliedForce() const
	{
		return appliedForce;
	}

	inline void SetAppliedForce(NCLVector3 appliedForce)
	{
		this->appliedForce = appliedForce;
	}

	inline void ApplyImpulse(NCLVector3 impulse)
	{
		linVelocity += impulse;
	}

	const NCLMatrix4&				GetWorldSpaceTransform()    const { return worldTransform; }




	//<--------- SETTERS ------------->
	inline void SetParent(GameObject* obj)							{ parent = obj; }

	inline void SetElasticity(float elasticityCoeff)				{ elasticity = elasticityCoeff; }
	inline void SetFriction(float frictionCoeff)					{ friction = frictionCoeff; }

	inline void SetPosition(const NCLVector3& v)						{ position = v; FireOnUpdateCallback(); }
	inline void SetLinearVelocity(const NCLVector3& v)					{ linVelocity = v; }
	inline void SetForce(const NCLVector3& v)							{ force = v; }
	inline void SetAcceleration(const NCLVector3& v) { acceleration = v; }
	inline void SetInverseMass(const float& v)						
	{ 
		invMass = v, 0.5f;
	}

	inline void SetOrientation(const Quaternion& v)					{ orientation = v; FireOnUpdateCallback(); }
	inline void SetAngularVelocity(const NCLVector3& v)				{ angVelocity = v; }
	inline void SetTorque(const NCLVector3& v)							{ torque = v; }
	inline void SetInverseInertia(const NCLMatrix3& v)					{ invInertia = v; }

	void SetRotation(NCLVector4 rotation)
	{
		worldTransform = (NCLMatrix4::Translation(position) *
			NCLMatrix4::Rotation(rotation.w, NCLVector3(rotation.x, rotation.y, rotation.z)) *
			NCLMatrix4::Scale(worldTransform.GetScalingVector()));

		orientation = Quaternion::AxisAngleToQuaterion(NCLVector3(rotation.x, rotation.y, rotation.z), rotation.w);
	}

	inline void AddCollisionShape(CollisionShape* colShape)
	{ 
		//if (collisionShapes[index]) collisionShapes[index]->SetParent(NULL);
		//collisionShapes[index] = colShape;
		//if (collisionShapes[index]) collisionShapes[index]->SetParent(this);
		collisionShapes.push_back(colShape);
		//colShape->SetParent(this);
	}

	inline void AddCollisionShapes(std::vector<CollisionShape*>* colShapes)
	{
		//if (collisionShapes[index]) collisionShapes[index]->SetParent(NULL);
		//collisionShapes[index] = colShape;
		//if (collisionShapes[index]) collisionShapes[index]->SetParent(this);
		for each (CollisionShape* colShape in *colShapes)
		{
			collisionShapes.push_back(colShape);
			//colShape->SetParent(this);
		}
	}
	
	inline void SetCollisionShapes(std::vector<CollisionShape*> shapes)
	{
		collisionShapes = shapes;
	}


	//<---------- CALLBACKS ------------>
	inline void SetOnCollisionCallback(PhysicsCollisionCallback callback) { onCollisionCallback = callback; }
	inline bool FireOnCollisionEvent(PhysicsNode* obj_a, PhysicsNode* obj_b, CollisionData collisionData)
	{
		return (onCollisionCallback) ? onCollisionCallback(obj_a, obj_b, collisionData) : true;
	}

	inline void SetOnUpdateCallback(PhysicsUpdateCallback callback)
	{
		onUpdateCallbacks.push_back(callback);
	}

	inline void FireOnUpdateCallback()
	{
		//Build world transform
		worldTransform = orientation.ToMatrix4();
		worldTransform.SetPositionVector(position);

		if (worldTransform.GetPositionVector() != previousTransform.GetPositionVector())
		{
			for each (PhysicsUpdateCallback callback in onUpdateCallbacks)
			{
				//Fire the OnUpdateCallback, notifying GameObject's and other potential
				// listeners that this PhysicsNode has a new world transform.
				if (callback) callback(worldTransform);
			}

			movedSinceLastBroadPhase = true;
		}

		previousTransform = worldTransform;
	}
	
	bool movedSinceLastBroadPhase = false;
	bool toDeleteInOctree = false;
	bool collidable = false;
	bool transmitCollision = false;
	bool multipleTransmitions = false;
	bool hasTransmittedCollision = false;
	bool constantAcceleration = false;

	std::string collisionShapeType;

	TrackedMessageSender<CollisionMessage> collisionMessageSender;

	std::vector<CollisionShape*> collisionShapes;
protected:
	GameObject*				parent;
	NCLMatrix4					worldTransform;
	std::vector<PhysicsUpdateCallback>	onUpdateCallbacks;


	//<---------LINEAR-------------->
	NCLVector3		position;
	NCLVector3		linVelocity;
	NCLVector3		force;
	NCLVector3		acceleration;
	float		invMass;
	NCLVector3 appliedForce;

	//<----------ANGULAR-------------->
	Quaternion  orientation;
	NCLVector3		angVelocity;
	NCLVector3		torque;
	NCLMatrix3     invInertia;


	//<----------COLLISION------------>
	PhysicsCollisionCallback	 onCollisionCallback;


	//<--------MATERIAL-------------->
	float				elasticity;		///Value from 0-1 definiing how much the object bounces off other objects
	float				friction;		///Value from 0-1 defining how much the object can slide off other objects
	float	damping;


	NCLMatrix4 previousTransform;

	bool enabled;
	bool isCollision;
	bool isStatic;
};