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

void Manifold::initiate(PhysicsNode* nodeA, PhysicsNode* nodeB)
{
	contactPoints.clear();

	pnodeA = nodeA;
	pnodeB = nodeB;
}

void Manifold::applyImpulse()
{
	for (ContactPoint& contact : contactPoints)
	{
		solveContactPoint(contact);
	}
}


void Manifold::solveContactPoint(ContactPoint& c)
{
	NCLVector3 r1 = c.relPosA;
	NCLVector3 r2 = c.relPosB;

	NCLVector3 v0 = pnodeA->getLinearVelocity() + NCLVector3::cross(pnodeA->getAngularVelocity(), r1);
	NCLVector3 v1 = pnodeB->getLinearVelocity() + NCLVector3::cross(pnodeB->getAngularVelocity(), r2);

	NCLVector3 dv = v1 - v0;

	float constraintMass = (pnodeA->getInverseMass() + pnodeB->getInverseMass()) +
		NCLVector3::dot(c.colNormal,
			NCLVector3::cross(pnodeA->getInverseInertia() * NCLVector3::cross(r1, c.colNormal), r1) +
			NCLVector3::cross(pnodeB->getInverseInertia() * NCLVector3::cross(r2, c.colNormal), r2)
		);

	if (constraintMass > 0.f) {
		float jn = max(-NCLVector3::dot(dv, c.colNormal) + c.bTerm, 0.f);

		float oldSumImpulseContact = c.sumImpulseContact;
		c.sumImpulseContact = max(c.sumImpulseContact + jn, 0.f);
		jn = c.sumImpulseContact - oldSumImpulseContact;


		jn = jn / constraintMass;

		pnodeA->setLinearVelocity(pnodeA->getLinearVelocity() - c.colNormal*(jn * pnodeA->getInverseMass()));
		pnodeB->setLinearVelocity(pnodeB->getLinearVelocity() + c.colNormal*(jn * pnodeB->getInverseMass()));

		pnodeA->setAngularVelocity(pnodeA->getAngularVelocity() - pnodeA->getInverseInertia() * NCLVector3::cross(r1, c.colNormal * jn));
		pnodeB->setAngularVelocity(pnodeB->getAngularVelocity() + pnodeB->getInverseInertia() * NCLVector3::cross(r2, c.colNormal * jn));

	}

	NCLVector3 tangent = dv - c.colNormal * NCLVector3::dot(dv, c.colNormal);
	float tangent_len = tangent.length();

	if (tangent_len > 1e-6f) {
		tangent = tangent / tangent_len;
		float frictionalMass = (pnodeA->getInverseMass() + pnodeB->getInverseMass()) +
			NCLVector3::dot(tangent,
				NCLVector3::cross(pnodeA->getInverseInertia() * NCLVector3::cross(r1, tangent), r1) +
				NCLVector3::cross(pnodeB->getInverseInertia() * NCLVector3::cross(r2, tangent), r2)
			);

		if (frictionalMass > 0.f) {
			float frictionCoef = pnodeA->getFriction() * pnodeB->getFriction();

			float jt = -NCLVector3::dot(dv, tangent) * frictionCoef;

			NCLVector3 oldImpulseFriction = c.sumImpulseFriction;
			c.sumImpulseFriction = c.sumImpulseFriction + tangent * jt;
			float len = c.sumImpulseFriction.length();

			if (len > 0.f && len > c.sumImpulseContact) {
				c.sumImpulseFriction = c.sumImpulseFriction / len * c.sumImpulseContact;
			}

			tangent = c.sumImpulseFriction - oldImpulseFriction;
			jt = 1.f;

			jt = jt / frictionalMass;

			pnodeA->setLinearVelocity(pnodeA->getLinearVelocity() - tangent*(jt*pnodeA->getInverseMass()));
			pnodeB->setLinearVelocity(pnodeB->getLinearVelocity() + tangent*(jt*pnodeB->getInverseMass()));

			pnodeA->setAngularVelocity(pnodeA->getAngularVelocity() - pnodeA->getInverseInertia() * NCLVector3::cross(r1, tangent * jt));
			pnodeB->setAngularVelocity(pnodeB->getAngularVelocity() + pnodeB->getInverseInertia() * NCLVector3::cross(r2, tangent * jt));
		}
	}
}

void Manifold::preSolverStep(float dt)
{
	for (ContactPoint& contact : contactPoints)
	{
		updateConstraint(contact, dt);
	}
}

void Manifold::updateConstraint(ContactPoint& c, float deltaTime)
{
	//Reset total impulse forces computed this physics timestep 
	c.sumImpulseContact = 0.0f;
	c.sumImpulseFriction = NCLVector3(0.0f, 0.0f, 0.0f);
	c.bTerm = 0.0f;

	const float baumgarte_scalar = 0.1f;
	const float baumgarte_slop = 0.001f;
	const float penetration_slop = min(c.colPenetration + baumgarte_slop, 0.0f);

	c.bTerm += -(baumgarte_scalar / deltaTime) * penetration_slop;

	const float elasticity = pnodeA->getElasticity() * pnodeB->getElasticity();
	const float elasticity_term = NCLVector3::dot(c.colNormal,
		pnodeA->getLinearVelocity() +
		NCLVector3::cross(c.relPosA, pnodeA->getAngularVelocity()) -
		pnodeB->getLinearVelocity() -
		NCLVector3::cross(c.relPosB, pnodeB->getAngularVelocity()));

	c.bTerm += (elasticity * elasticity_term) / contactPoints.size();
}

void Manifold::addContact(const NCLVector3& globalOnA, const NCLVector3& globalOnB, const NCLVector3& normal, const float& penetration)
{
	//Get relative offsets from each object centre of mass
	// Used to compute rotational velocity at the point of contact.
	NCLVector3 r1 = (globalOnA - pnodeA->getPosition());
	NCLVector3 r2 = (globalOnB - pnodeB->getPosition());

	//Create our new contact descriptor
	ContactPoint contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.colNormal = normal;
	contact.colNormal.normalise();
	contact.colPenetration = penetration;

	contactPoints.push_back(contact);
}
