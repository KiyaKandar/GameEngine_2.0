#include "PhysicsNode.h"

#include "PhysicsEngine.h"
#include "SemiImplicitEuler.h"

void PhysicsNode::IntegrateForVelocity(float dt)
{
	linVelocity = SemiImplicitEuler::UpdateLinearVelocity(this, dt);
	appliedForce.toZero();

	angVelocity = SemiImplicitEuler::UpdateAngularVelocity(this, dt);

	for each (CollisionShape* shape in collisionShapes)
	{
		shape->Parent()->SetLinearVelocity(linVelocity);
		shape->Parent()->SetAngularVelocity(angVelocity);
	}
}

/* Between these two functions the physics engine will solve for velocity
   based on collisions/constraints etc. So we need to integrate velocity, solve 
   constraints, then use final velocity to update position. 
*/

void PhysicsNode::IntegrateForPosition(float dt)
{
	position = SemiImplicitEuler::UpdateDisplacement(this, dt);
	orientation = SemiImplicitEuler::UpdateOrentation(this, dt);
	orientation.normalise();

	for each (CollisionShape* shape in collisionShapes)
	{
		shape->Parent()->SetPosition(SemiImplicitEuler::UpdateDisplacement(shape->Parent(), dt));
		shape->Parent()->SetOrientation(SemiImplicitEuler::UpdateOrentation(shape->Parent(), dt));
	}

	//Finally: Notify any listener's that this PhysicsNode has a new world transform.
	// - This is used by GameObject to set the worldTransform of any RenderNode's. 
	//   Please don't delete this!!!!!
	FireOnUpdateCallback();
}