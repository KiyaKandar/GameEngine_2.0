#include "Manifold.h"

#include "../Utilities/Maths/Matrix3.h"
#include "PhysicsEngine.h"
#include <algorithm>

Manifold::Manifold() 
	: pnodeA(NULL)
	, pnodeB(NULL)
{
}

Manifold::~Manifold()
{

}

void Manifold::Initiate(PhysicsNode* nodeA, PhysicsNode* nodeB)
{
	contactPoints.clear();

	pnodeA = nodeA;
	pnodeB = nodeB;
}

void Manifold::ApplyImpulse()
{
	for (ContactPoint& contact : contactPoints)
	{
		SolveContactPoint(contact);
	}
}


void Manifold::SolveContactPoint(ContactPoint& c)
{	
	NCLVector3 r1 = c.relPosA;
	NCLVector3 r2 = c.relPosB;

	NCLVector3 v0 = pnodeA->GetLinearVelocity()
		+ NCLVector3::cross(pnodeA->GetAngularVelocity(), r1);
	NCLVector3 v1 = pnodeB->GetLinearVelocity()
		+ NCLVector3::cross(pnodeB->GetAngularVelocity(), r2);

	NCLVector3 dv = v1 - v0;

	//Collision Resolution
	float constraintMass =
		(pnodeA->GetInverseMass() + pnodeB->GetInverseMass()) +
		NCLVector3::dot(c.colNormal,
			NCLVector3::cross(pnodeA->GetInverseInertia()
				* NCLVector3::cross(r1, c.colNormal), r1) +
			NCLVector3::cross(pnodeB->GetInverseInertia()
				* NCLVector3::cross(r2, c.colNormal), r2));

	if (constraintMass > 0.0f)
	{
		float jn = max(-NCLVector3::dot(dv, c.colNormal) + c.b_term, 0.0f);

		float oldSumImpulseContact = c.sumImpulseContact;
		c.sumImpulseContact = max(c.sumImpulseContact + jn, 0.0f);
		jn = c.sumImpulseContact - oldSumImpulseContact;

		jn = jn / constraintMass;

		pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
			- c.colNormal * (jn * pnodeA->GetInverseMass()));
		pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
			+ c.colNormal * (jn * pnodeB->GetInverseMass()));

		pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
			- pnodeA->GetInverseInertia()
			* NCLVector3::cross(r1, c.colNormal * jn));
		pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
			+ pnodeB->GetInverseInertia()
			* NCLVector3::cross(r2, c.colNormal * jn));
	}

	//Friction
	NCLVector3 tangent = dv - c.colNormal * NCLVector3::dot(dv, c.colNormal);
	float tangent_len = tangent.length();

	if (tangent_len > 1e-6f)
	{
		tangent = tangent / tangent_len;

		float frictionalMass = (pnodeA->GetInverseMass()
			+ pnodeB->GetInverseMass()) + NCLVector3::dot(tangent,
				NCLVector3::cross(pnodeA->GetInverseInertia()
					* NCLVector3::cross(r1, tangent), r1) +
				NCLVector3::cross(pnodeB->GetInverseInertia()
					* NCLVector3::cross(r2, tangent), r2));

		if (frictionalMass > 0.0f)
		{
			float frictionCoef = (pnodeA->GetFriction() * pnodeB->GetFriction());
			float jt = -NCLVector3::dot(dv, tangent) * frictionCoef;

			NCLVector3 oldImpulseFriction = c.sumImpulseFriction;
			c.sumImpulseFriction = c.sumImpulseFriction + tangent * jt;
			float len = c.sumImpulseFriction.length();

			if (len > 0.0f && len > c.sumImpulseContact)
			{
				c.sumImpulseFriction =
					c.sumImpulseFriction / len * c.sumImpulseContact;
			}

			tangent = c.sumImpulseFriction - oldImpulseFriction;
			jt = 1.0f;

			jt = jt / frictionalMass;

			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				- tangent * (jt * pnodeA->GetInverseMass()));
			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				+ tangent * (jt * pnodeB->GetInverseMass()));

			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				- pnodeA->GetInverseInertia()
				* NCLVector3::cross(r1, tangent * jt));
			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				+ pnodeB->GetInverseInertia()
				* NCLVector3::cross(r2, tangent * jt));
		}
	}
}

void Manifold::PreSolverStep(float dt)
{
	std::random_shuffle(contactPoints.begin(), contactPoints.end());

	for (ContactPoint& contact : contactPoints)
	{
		UpdateConstraint(contact, dt);
	}
}

void Manifold::UpdateConstraint(ContactPoint& c, const float deltaTime)
{
	//Reset total impulse forces computed this physics timestep 
	c.sumImpulseContact = 0.0f;
	c.sumImpulseFriction = NCLVector3(0.0f, 0.0f, 0.0f);
	c.b_term = 0.0f;

	const float baumgarte_scalar = 0.3f;
	const float baumgarte_slop = 0.3f;
	const float penetration_slop = min(c.colPenetration + baumgarte_slop, 0.0f);

	c.b_term +=
		-(baumgarte_scalar / deltaTime)
		* penetration_slop;

	const float elasticity =
		pnodeA->GetElasticity() * pnodeB->GetElasticity();
	const float elasticity_term = NCLVector3::dot(c.colNormal,
		pnodeA->GetLinearVelocity()
		+ NCLVector3::cross(c.relPosA, pnodeA->GetAngularVelocity())
		- pnodeB->GetLinearVelocity()
		- NCLVector3::cross(c.relPosB, pnodeB->GetAngularVelocity()));

	c.b_term += (elasticity * elasticity_term) / contactPoints.size();
}

void Manifold::AddContact(const NCLVector3& globalOnA, const NCLVector3& globalOnB, const NCLVector3& normal, const float& penetration)
{
	//Get relative offsets from each object centre of mass
	// Used to compute rotational velocity at the point of contact.
	NCLVector3 r1 = (globalOnA - pnodeA->GetPosition());
	NCLVector3 r2 = (globalOnB - pnodeB->GetPosition());

	//Create our new contact descriptor
	ContactPoint contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.colNormal = normal;
	contact.colNormal.normalise();
	contact.colPenetration = penetration;

	contactPoints.push_back(contact);


	//What a stupid function!
	// - Manifold's normally persist over multiple frames, as in two colliding objects
	//   (especially in the case of stacking) will likely be colliding in a similar 
	//   setup the following couple of frames. So the accuracy therefore can be increased
	//   by using persistent manifolds, which will only be deleted when the two objects
	//   fail a narrowphase check. This means the manifolds can be quite busy, with lots of
	//   new contacts per frame, but to solve any convex manifold in 3D you really only need
	//   3 contacts (4 max), so tldr; perhaps you want persistent manifolds.. perhaps
	//   you want to put some code here to sort contact points.. perhaps this comment is even 
	//   more pointless than a passthrough function.. perhaps I should just stop typ
}

void Manifold::DebugDraw(std::vector<DebugLineMessage>& lineMessages, std::vector<DebugSphereMessage>& sphereMessages) const
{
	if (contactPoints.size() > 0)
	{
		//Loop around all contact points and draw them all as a line-loop
		NCLVector3 globalOnA1 = pnodeA->GetPosition() + contactPoints.back().relPosA;
		for (const ContactPoint& contact : contactPoints)
		{
			NCLVector3 globalOnA2 = pnodeA->GetPosition() + contact.relPosA;
			NCLVector3 globalOnB = pnodeB->GetPosition() + contact.relPosB;

			//Draw line to form area given by all contact points
			lineMessages.push_back(DebugLineMessage("RenderingSystem", globalOnA1, globalOnA2, NCLVector3(0.0f, 1.0f, 0.0f)));

			//Draw descriptors for indivdual contact point
			sphereMessages.push_back(DebugSphereMessage("RenderingSystem", globalOnA2, 0.05f, NCLVector3(0.0f, 0.5f, 0.0f)));
			lineMessages.push_back(DebugLineMessage("RenderingSystem", globalOnB, globalOnA2, NCLVector3(1.0f, 0.0f, 1.0f)));

			globalOnA1 = globalOnA2;
		}
	}
}