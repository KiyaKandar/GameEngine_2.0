#include "PhysicsEngine.h"
#include "CollisionDetectionSAT.h"

#include "OctreePartitioning.h"

#include "../Communication/Messages/ApplyForceMessage.h"
#include "../Communication/Messages/CollisionMessage.h"
#include "../Utilities/GameTimer.h"
#include "../Communication/Messages/AbsoluteTransformMessage.h"
#include "../Input/Devices/Keyboard.h"

#pragma optimize("", off)

PhysicsEngine::PhysicsEngine(Database* database, Keyboard* keyboard) : Subsystem("Physics")
{
	this->database = database;
	this->keyboard = keyboard;

	std::vector<MessageType> types = { MessageType::TEXT, MessageType::PLAYER_INPUT, MessageType::RELATIVE_TRANSFORM, 
		MessageType::APPLY_FORCE, MessageType::APPLY_IMPULSE, MessageType::UPDATE_POSITION, MessageType::ABSOLUTE_TRANSFORM,
		MessageType::MOVE_GAMEOBJECT, MessageType::SCALE_GAMEOBJECT, MessageType::ROTATE_GAMEOBJECT, MessageType::TOGGLE_GAMEOBJECT};

	incomingMessages = MessageProcessor(types, DeliverySystem::getPostman()->getDeliveryPoint("Physics"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TOGGLE_GAMEOBJECT, [database = database](Message* message)
	{
		ToggleGameObjectMessage* toggleMessage = static_cast<ToggleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(toggleMessage->gameObjectID));

		gameObject->getPhysicsNode()->setEnabled(toggleMessage->isEnabled);
		//gameObject->setEnabled(toggleMessage->isEnabled);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [database = database, this](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addphysicsnode")
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->getTable("GameObjects")->getResource(tokens[1]));

			addPhysicsObject(gameObject->getPhysicsNode());
		}
		else if (tokens[0] == "removephysicsnode")
		{
			for (auto physicsNodeiterator = physicsNodes.begin(); physicsNodeiterator != physicsNodes.end(); ++physicsNodeiterator)
			{
				if ((*physicsNodeiterator)->getParent()->getName() == tokens[1])
				{
					physicsNodes.erase(physicsNodeiterator);
					break;
				}
			}
		}
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::ABSOLUTE_TRANSFORM, [database = database](Message* message)
	{
		AbsoluteTransformMessage* translationMessage = static_cast<AbsoluteTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(translationMessage->resourceName));

		gameObject->getPhysicsNode()->setPosition(translationMessage->transform.getPositionVector());
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::MOVE_GAMEOBJECT, [database = database](Message* message)
	{
		MoveGameObjectMessage* moveMessage = static_cast<MoveGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(moveMessage->gameObjectID));

		gameObject->getPhysicsNode()->setPosition(moveMessage->position);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::SCALE_GAMEOBJECT, [database = database](Message* message)
	{
		ScaleGameObjectMessage* scaleMessage = static_cast<ScaleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(scaleMessage->gameObjectID));

		gameObject->getPhysicsNode()->getCollisionShape()->setScale(scaleMessage->scale, gameObject->getPhysicsNode()->getInverseMass());
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::ROTATE_GAMEOBJECT, [database = database](Message* message)
	{
		RotateGameObjectMessage* rotateMessage = static_cast<RotateGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(rotateMessage->gameObjectID));

		if (rotateMessage->relative)
		{
			gameObject->getPhysicsNode()->setOrientation(gameObject->getPhysicsNode()->getOrientation() *
				Quaternion::axisAngleToQuaterion(NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z), rotateMessage->rotation.w));
		}
		else
		{
			gameObject->getPhysicsNode()->setOrientation(
				Quaternion::axisAngleToQuaterion(NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z), rotateMessage->rotation.w));
		}
	});


	float dt = getDeltaTime();

	incomingMessages.addActionToExecuteOnMessage(MessageType::APPLY_FORCE, [database/*, this*/](Message* message)
	{
		ApplyForceMessage* applyForceMessage = static_cast<ApplyForceMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(applyForceMessage->gameObjectID));

		NCLVector3 force = applyForceMessage->force /* * this->getDeltaTime() * 1000*/;

		if (applyForceMessage->isRandom)
		{
			if (applyForceMessage->xmin != applyForceMessage->xmax)
			{
				force.x = VectorBuilder::getRandomVectorComponent(applyForceMessage->xmin, applyForceMessage->xmax) * 50.0f; 
				//These * 50.0f are needed because currently rand() doesn't give good results for large ranges. 
				//So a smaller range is needed when choosing random min and max values for vectors, which should then be scaled to the appropriate value
				//Get rid of them though as any random force component will now be scaled and this isn't good!
			}
			if (applyForceMessage->ymin != applyForceMessage->ymax)
			{
				force.y = VectorBuilder::getRandomVectorComponent(applyForceMessage->ymin, applyForceMessage->ymax) * 50.0f;
			}
			if (applyForceMessage->zmin != applyForceMessage->zmax)
			{
				force.z = VectorBuilder::getRandomVectorComponent(applyForceMessage->zmin, applyForceMessage->zmax) * 50.0f;
			}
		}

		gObj->getPhysicsNode()->setAppliedForce(force);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::APPLY_IMPULSE, [database](Message* message)
	{
		ApplyImpulseMessage* applyImpulseMessage = static_cast<ApplyImpulseMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(applyImpulseMessage->gameObjectID));

		NCLVector3 impulse = applyImpulseMessage->impulse;

		if (applyImpulseMessage->isRandom)
		{
			if (applyImpulseMessage->xmin != applyImpulseMessage->xmax)
			{
				impulse.x = VectorBuilder::getRandomVectorComponent(applyImpulseMessage->xmin, applyImpulseMessage->xmax);
			}
			if (applyImpulseMessage->ymin != applyImpulseMessage->ymax)
			{
				impulse.y = VectorBuilder::getRandomVectorComponent(applyImpulseMessage->ymin, applyImpulseMessage->ymax);
			}
			if (applyImpulseMessage->zmin != applyImpulseMessage->zmax)
			{
				impulse.z = VectorBuilder::getRandomVectorComponent(applyImpulseMessage->zmin, applyImpulseMessage->zmax);
			}
		}
		gObj->getPhysicsNode()->applyImpulse(impulse);
	});

	

	incomingMessages.addActionToExecuteOnMessage(MessageType::UPDATE_POSITION, [database/*, &dt*/](Message* message)
	{
		UpdatePositionMessage* positionMessage = static_cast<UpdatePositionMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(positionMessage->gameObjectID));

		gObj->getPhysicsNode()->setPosition((positionMessage->position)/**dt*/);
	});

	updateTimestep = 1.0f / 60.f;
	updateRealTimeAccum = 0.0f;

	timer->addChildTimer("Broadphase");
	timer->addChildTimer("Narrowphase");
	timer->addChildTimer("Solver");
	timer->addChildTimer("Integrate Position");
	timer->addChildTimer("Integrate Velocity");
}


PhysicsEngine::~PhysicsEngine()
{
	removeAllPhysicsObjects();
}

void PhysicsEngine::addPhysicsObject(PhysicsNode * obj)
{


	physicsNodes.push_back(obj);
	if (obj->getEnabled())
	{
		BpOct.sortNode(obj);
	}
	

	obj->setOnCollisionCallback([](PhysicsNode* this_obj, PhysicsNode* colliding_obj, CollisionData collisionData)
	{
		if (this_obj->transmitCollision)
		{
			if (!this_obj->hasTransmittedCollision)
			{
				if (this_obj->collisionMessageSender.readyToSendNextMessage())
				{
					this_obj->collisionMessageSender.setMessage(CollisionMessage("Gameplay", collisionData,
						this_obj->getParent()->getName(), colliding_obj->getParent()->getName()));
					this_obj->collisionMessageSender.sendMessage();

					if (!this_obj->multipleTransmitions)
					{
						this_obj->hasTransmittedCollision = true;
					}
				}
			}
			
			return true;
		}
		return true;
	});

	obj->setOnUpdateCallback(std::bind(
		&PhysicsEngine::OctreeChanged,
		this,
		std::placeholders::_1));
}

void PhysicsEngine::removePhysicsObject(PhysicsNode * obj)
{
	//Lookup the object in question
	auto found_loc = std::find(physicsNodes.begin(), physicsNodes.end(), obj);

	//If found, remove it from the list
	if (found_loc != physicsNodes.end())
	{
		physicsNodes.erase(found_loc);
	}
}

void PhysicsEngine::removeAllPhysicsObjects()
{
	for (Constraint* c : constraints)
	{
		delete c;
	}
	constraints.clear();

	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();


	for (PhysicsNode* obj : physicsNodes)
	{
		if (obj->getParent())
			obj->getParent()->setPhysicsNode(nullptr);
		delete obj;
	}
	physicsNodes.clear();
	BpOct.clear();
}

void PhysicsEngine::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	const int maxUpdatesPerFrame = 5;
	
	updateRealTimeAccum += deltaTime * 0.001f;
	for (int i = 0; (updateRealTimeAccum >= updateTimestep) && i < maxUpdatesPerFrame; ++i)
	{
		updateRealTimeAccum -= updateTimestep;

		updatePhysics();
	}

	if (updateRealTimeAccum >= updateTimestep)
	{
		updateRealTimeAccum = 0.0f;
	}

	for (PhysicsNode* physicsNode : physicsNodes)
	{
		physicsNode->hasTransmittedCollision = false;
	}

	if (keyboard->keyTriggered(KEYBOARD_F8))
	{
		wireframeRendering = !wireframeRendering;
	}

	if (wireframeRendering)
	{
		if (cubeDrawMessageSender.readyToSendNextMessageGroup() && sphereDrawMessageSender.readyToSendNextMessageGroup())
		{
			std::vector<DebugLineMessage> cubeDrawMessages;
			std::vector<DebugSphereMessage> sphereDrawMessages;

			BpOct.DebugDraw(cubeDrawMessages);

			for (PhysicsNode* node : physicsNodes)
			{
				node->getCollisionShape()->debugDraw(cubeDrawMessages, sphereDrawMessages);
			}

			cubeDrawMessageSender.setMessageGroup(cubeDrawMessages);
			sphereDrawMessageSender.setMessageGroup(sphereDrawMessages);

			cubeDrawMessageSender.sendMessageGroup();
			sphereDrawMessageSender.sendMessageGroup();
		}
	}

	timer->endTimedSection();
}

void PhysicsEngine::updatePhysics()
{
	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();

	
	//A whole physics engine in 6 simple steps =D

	//-- Using positions from last frame --
	//1. Broadphase Collision Detection (Fast and dirty)
	timer->beginChildTimedSection("Broadphase");
	broadPhaseCollisions();
	timer->endChildTimedSection("Broadphase");
	
	//2. Narrowphase Collision Detection (Accurate but slow)
	timer->beginChildTimedSection("Narrowphase");
	narrowPhaseCollisions();
	timer->endChildTimedSection("Narrowphase");
	
	//3. Initialize Constraint Params (precompute elasticity/baumgarte factor etc)	
	for (Manifold* m : manifolds)
	{
		m->preSolverStep(updateTimestep);
	}

	for (Constraint* c : constraints)
	{
		c->preSolverStep(updateTimestep);
	}


	//4. Update Velocities
	timer->beginChildTimedSection("Integrate Velocity");
	for (PhysicsNode* obj : physicsNodes) 
	{
		if(obj->getEnabled())
			obj->integrateForVelocity(updateTimestep);
	}
	timer->endChildTimedSection("Integrate Velocity");
	
	//5. Constraint Solver
	timer->beginChildTimedSection("Solver");
	for (size_t i = 0; i < SOLVER_ITERATIONS; ++i) 
	{
		for (Manifold* m : manifolds) m->applyImpulse();
		for (Constraint* c : constraints) c->applyImpulse(updateTimestep);
	}
	timer->endChildTimedSection("Solver");
	
	//6. Update Positions (with final 'real' velocities)
	timer->beginChildTimedSection("Integrate Position");
	for (PhysicsNode* obj : physicsNodes)
	{
		if (obj->getEnabled())
			obj->integrateForPosition(updateTimestep);
	}
	timer->endChildTimedSection("Integrate Position");
	
}

void PhysicsEngine::broadPhaseCollisions()
{
	broadphaseColPairs.clear();

	PhysicsNode* nodeA;
	PhysicsNode* nodeB;

	for (PhysicsNode * p : physicsNodes) {
		if (p->getLinearVelocity().lengthSquared() > 0 && p->getEnabled()) {
			BpOct.sortNode(p);
		}
	}

	if (physicsNodes.size() > 0)
	{
		for (BParea &p : BpOct.getBpAreas()) 
		{

			for (size_t i = 0; i < p.nodesInArea.size() - 1 && p.nodesInArea.size() > 0; ++i)
			{
				for (size_t j = i + 1; j < p.nodesInArea.size(); ++j)
				{
					if (p.nodesInArea[i]->getEnabled())
					{
						nodeA = p.nodesInArea[i];
					}
					else 
					{
						continue;
					}
					
					if (p.nodesInArea[j]->getEnabled())
					{
						nodeB = p.nodesInArea[j];
					}
					else
					{
						continue;
					}

					//lets check collisions with broadphase shapes
				

					//Check they both atleast have collision shapes

					if (!(nodeA->getIsStatic() && nodeB->getIsStatic()))
					{
							if (BroadPhaseCulling::SphereSphereCollision(nodeA, nodeB))
							{
								CollisionPair cp;
								cp.pObjectA = nodeA;
								cp.pObjectB = nodeB;

								broadphaseColPairs.insert(cp);
							}
					}
				}
			}
		}
	}
}

void PhysicsEngine::narrowPhaseCollisions()
{
	if (broadphaseColPairs.size() > 0)
	{
		CollisionData colData;

		CollisionDetectionSAT colDetect;

		for (CollisionPair cp : broadphaseColPairs)
		{

			CollisionShape *shapeA = cp.pObjectA->getCollisionShape();
			CollisionShape *shapeB = cp.pObjectB->getCollisionShape();

			colDetect.beginNewPair(
				cp.pObjectA,
				cp.pObjectB,
				cp.pObjectA->getCollisionShape(),
				cp.pObjectB->getCollisionShape());

			
			if (colDetect.areColliding(&colData))
			{
				bool okA = cp.pObjectA->fireOnCollisionEvent(cp.pObjectA, cp.pObjectB, colData);
				bool okB = cp.pObjectB->fireOnCollisionEvent(cp.pObjectB, cp.pObjectA, colData);

				if (okA && okB)
				{
					Manifold* manifold = new Manifold();
					manifold->initiate(cp.pObjectA, cp.pObjectB);

					colDetect.genContactPoints(manifold);

					if (manifold->contactPoints.size() > 0) 
					{
						manifolds.push_back(manifold);
					}
					else {
						delete manifold;
					}
				}
			}
		}

	}
}

void PhysicsEngine::InitialiseOctrees(int entityLimit)
{
	BpOct.initBroadphase(NCLVector3(-300,-300,-300),NCLVector3(300,300,300));
}
