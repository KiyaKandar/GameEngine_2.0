#include "PaintGameActionBuilder.h"

#include "../../Resource Management/XMLParser.h"
#include "../../Resource Management/Database/Database.h"
#include "../../Communication/DeliverySystem.h"
#include "../../Communication/Messages/PaintTrailForGameObjectMessage.h"
#include "../../Communication/SendMessageActionBuilder.h"
#include "../../Communication/Messages/ToggleGameObjectMessage.h"
#include "../GameObject.h"
#include "../../Physics/PhysicsNode.h"

#include <iostream>
#include <random>

std::unordered_map<std::string, Builder>PaintGameActionBuilder::builders
	= std::unordered_map<std::string, Builder>();
std::string PaintGameActionBuilder::powerUpBuilders[2] = { "ScalePlayer", "DecreaseMass" };
Database* PaintGameActionBuilder::database = nullptr;
std::string PaintGameActionBuilder::localPlayer = "";
bool PaintGameActionBuilder::online = false;
int PaintGameActionBuilder::r1 = 0;
int PaintGameActionBuilder::others[10] = { 0, 0, 0, 0 , 0 , 0 , 0 , 0 , 0 , 0 };
int PaintGameActionBuilder::r1ToSet = 0;
int PaintGameActionBuilder::othersToSet[10] = { 0, 0, 0, 0 , 0 , 0 , 0 , 0 , 0 , 0 };

void PaintGameActionBuilder::InitialiseBuilders(Database* database)
{
	PaintGameActionBuilder::database = database;
	std::random_device rd;     // only used once to initialise (seed) engine

	builders.insert({"MoveMinions", [](Node* node)
	{
		Executable sendMessageAction = SendMessageActionBuilder::BuildSendMessageAction(node->children[0]);
		return [sendMessageAction]()
		{
			if (PaintGameActionBuilder::localPlayer == "player0"
				|| !PaintGameActionBuilder::online)
			{
				sendMessageAction();
			}
		};
	} });

	builders.insert({ "TransmitMinion", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		
		return [gameObject]()
		{
			
				DeliverySystem::GetPostman()->InsertMessage(TextMessage("NetworkClient", "insertMinion " + gameObject->GetName()));

		};
	} });

	builders.insert({ "TransmitCollider", [](Node* node)
	{
		std::string collider = node->children[0]->value;
		return [collider]()
		{

			DeliverySystem::GetPostman()->InsertMessage(TextMessage("NetworkClient", "insertCollider " + collider));

		};
	} });

	builders.insert({ "Jump", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		float impulse = stof(node->children[1]->value);

		return [gameObject, impulse]()
		{
			if (gameObject->stats.canJump)
			{
				gameObject->GetPhysicsNode()->ApplyImpulse(NCLVector3(0.f, impulse, 0.f));
				gameObject->stats.canJump = false;
			}

		};
	} });

	builders.insert({ "LetJump", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		
		return [gameObject]()
		{
			gameObject->stats.canJump = true;
		};
	} });



	builders.insert({ "CheckPaint", [](Node* node)
	{
		Executable sendMessageAction = SendMessageActionBuilder::BuildSendMessageAction(node->children[1]);
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));

		return [sendMessageAction, gameObject]()
		{
			if (gameObject->stats.currentPaint > 0)
			{
				sendMessageAction();
			}
				
		};
	} });

	builders.insert({ "SetMaxPaint", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		int maxPaint = stoi(node->children[1]->value);

		return [gameObject, maxPaint]()
		{
			gameObject->stats.maxPaint = maxPaint;
			gameObject->stats.currentPaint = maxPaint;

		};
	} });

	builders.insert({ "CreateMeteorPool", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		int amount = stoi(node->children[1]->value);
		std::string meshName = node->children[2]->value;
		float size = stof(node->children[3]->value);
		std::string baseName = node->children[0]->value + "Meteor";

		return [gameObject, amount, meshName, size, baseName]()
		{
			Database* database = PaintGameActionBuilder::database;

			gameObject->stats.meteors = amount;

			for (int i = 0; i < amount; i++)
			{
				GameObject* meteor = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(baseName + std::to_string(i)));
				if (meteor == nullptr)
				{
					SceneNode* sceneNode = new SceneNode(static_cast<Mesh*>(database->GetTable("Meshes")->GetResource(meshName)));
					sceneNode->SetColour(gameObject->stats.colourToPaint);

					meteor = new GameObject();
					meteor->SetSize(sizeof(GameObject));
					meteor->SetName(baseName + std::to_string(i));
					meteor->SetSceneNode(sceneNode);
					meteor->stats.colourToPaint = gameObject->stats.colourToPaint;
					meteor->SetScale(NCLVector3(size, size, size));

					PhysicsNode* physicsNode = new PhysicsNode();
					physicsNode->SetParent(meteor);
					//physicsNode->transmitCollision = true;
					physicsNode->SetCollisionShape("Sphere");
					physicsNode->SetInverseMass(0.2f);
					physicsNode->SetInverseInertia(physicsNode->GetCollisionShape()->BuildInverseInertia(physicsNode->GetInverseMass()));
					physicsNode->SetStatic(false);
					meteor->SetPhysicsNode(physicsNode);

					meteor->SetPosition(gameObject->GetPosition() + NCLVector3((float)i * 10.f, 50.f, 0.f));
					meteor->SetRotation(NCLVector4(0.f, 0.f, 0.f, 0.f));
					
					meteor->SetEnabled(false);

					PaintGameActionBuilder::database->GetTable("GameObjects")->AddNewResource(meteor);
					DeliverySystem::GetPostman()->InsertMessage(TextMessage("RenderingSystem", "addscenenode " + meteor->GetName()));
					DeliverySystem::GetPostman()->InsertMessage(TextMessage("Physics", "addphysicsnode " + meteor->GetName()));
					DeliverySystem::GetPostman()->InsertMessage(TextMessage("RenderingSystem", "setupmeshgameobject " + meteor->GetName()));
				}
			}
			
			
		};
	} });

	builders.insert({ "ReducePaint", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));

		return [gameObject]()
		{

			if (gameObject->GetPhysicsNode()->GetLinearVelocity().Length() > 0.1f)
			{
				gameObject->stats.currentPaint -= 2;
				int paint = max(gameObject->stats.currentPaint, 0);

				float massDecrease = 1 - ((float)paint / (float)gameObject->stats.maxPaint);

				if (paint < 25)
				{
					float interpolationFactor = ((float)paint / (float)gameObject->stats.maxPaint) * 4;
					NCLVector3 interpolatedColour = NCLVector3::Interpolate(NCLVector3(1.f, 1.f, 1.f), gameObject->stats.colourToPaint.ToVector3(), interpolationFactor);
					gameObject->GetSceneNode()->SetColour(NCLVector4(interpolatedColour.x, interpolatedColour.y, interpolatedColour.z, 1.f));
				}

				gameObject->GetPhysicsNode()->SetInverseMass((gameObject->stats.defaultInvMass + massDecrease));

				gameObject->stats.currentPaint = paint;
			}
		};
	} });


	builders.insert({ "RegainPaint", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));


		return [gameObject]()
		{
			gameObject->GetPhysicsNode()->SetInverseMass(gameObject->stats.defaultInvMass);
			gameObject->stats.currentPaint = gameObject->stats.maxPaint;
			gameObject->GetSceneNode()->SetColour(gameObject->stats.colourToPaint);
		};
	} });

	builders.insert({ "RegainNetworkedPaint", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		std::string paintpool = node->children[1]->value;


		return [gameObject, paintpool]()
		{
			gameObject->GetPhysicsNode()->SetInverseMass(gameObject->stats.defaultInvMass);
			gameObject->stats.currentPaint = gameObject->stats.maxPaint;
			gameObject->GetSceneNode()->SetColour(gameObject->stats.colourToPaint);

			if (PaintGameActionBuilder::localPlayer == gameObject->GetName()
				&& PaintGameActionBuilder::online)
			{
				DeliverySystem::GetPostman()->InsertMessage(TextMessage("NetworkClient", "collision " + gameObject->GetName() + " " + paintpool));
			}
		};
	} });

	builders.insert({ "PaintMinion", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		GameObject* minion = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[1]->value));

		return [gameObject, minion]()
		{

			if (gameObject->stats.currentPaint > 0)
			{
				minion->GetSceneNode()->SetColour(gameObject->stats.colourToPaint);
				minion->stats.colourToPaint = gameObject->stats.colourToPaint;

				if (PaintGameActionBuilder::localPlayer == gameObject->GetName()
					&& PaintGameActionBuilder::online)
				{
					DeliverySystem::GetPostman()->InsertMessage(TextMessage("NetworkClient", "collision " + gameObject->GetName() + " " + minion->GetName()));
				}
			}
		};
	} });

	builders.insert({ "ScalePlayer", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		float multiplier = stof(node->children[1]->value);
		GameObject* powerup = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[2]->value));
		float duration = stof(node->children[3]->value) * 1000;

		return [gameObject, multiplier, powerup, duration]()
		{
			if (!gameObject->stats.executeAfter)
			{
				gameObject->SetPosition(NCLVector3(gameObject->GetPosition().x, gameObject->GetPosition().y + (gameObject->stats.defaultScale.y * multiplier * .5f), gameObject->GetPosition().z));
				gameObject->SetScale(gameObject->stats.defaultScale * multiplier);
				powerup->SetEnabled(false);


				gameObject->stats.timeToWait = duration;
				gameObject->stats.executeAfter = [gameObject, powerup]()
				{
					gameObject->SetScale(gameObject->stats.defaultScale);
					gameObject->stats.executeAfter = std::function<void()>();
					powerup->SetEnabled(true);
				};
			}
		};
	} });

	builders.insert({ "DecreaseMass", [](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		float multiplier = stof(node->children[1]->value); 
		GameObject* powerup = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[2]->value));
		float duration = stof(node->children[3]->value) * 1000;

		return [gameObject, multiplier, powerup, duration]()
		{
			if (!gameObject->stats.executeAfter)
			{
				gameObject->stats.defaultInvMass *= multiplier;
				powerup->SetEnabled(false);

				gameObject->stats.timeToWait = duration;
				gameObject->stats.executeAfter = [gameObject, powerup]()
				{
					gameObject->stats.defaultInvMass = 1.f;
					gameObject->stats.executeAfter = std::function<void()>();
					powerup->SetEnabled(true);

				};
			}
		};
	} });

	builders.insert({ "MeteorStrike", [&rd = rd](Node* node)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		GameObject* powerup = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[2]->value));
		float duration = stof(node->children[3]->value) * 1000;

		return [gameObject, powerup, duration, &rd = rd]()
		{
			if (!gameObject->stats.executeAfter)
			{
				powerup->SetEnabled(false);

				for (int i = 0; i < gameObject->stats.meteors; ++i)
				{
					GameObject* meteor = static_cast<GameObject*>(
						PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(gameObject->GetName() + "Meteor" + std::to_string(i)));

					std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
					std::uniform_int_distribution<int> uni(-6, 6); // guaranteed unbiased
					auto random_integer1 = uni(rng);
					auto random_integer2 = uni(rng);

					meteor->SetPosition(gameObject->GetPosition() + NCLVector3((float)random_integer1 * 10.f, 100.f + (i * 40.f), (float)random_integer2 * 10.f));
					meteor->SetEnabled(true);
				}

				gameObject->stats.timeToWait = duration;
				gameObject->stats.executeAfter = [gameObject, powerup]()
				{
					gameObject->stats.executeAfter = std::function<void()>();
					powerup->SetEnabled(true);

				};
			}
		};
	} });


	builders.insert({ "RandomPowerUp", [&rd = rd](Node* node)
	{
		
		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		float multiplier = stof(node->children[1]->value);
		GameObject* powerup = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[2]->value));
		float duration = stof(node->children[3]->value) * 1000;

		return [&rd = rd, gameObject, multiplier, powerup, duration]()
		{
			if (!gameObject->stats.executeAfter)
			{
				powerup->SetEnabled(false);
				gameObject->stats.timeToWait = duration;

				std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
				std::uniform_int_distribution<int> uni(0, 2); // guaranteed unbiased
				auto random_integer = uni(rng);

				switch (random_integer)
				{
					case SCALE_POWERUP:
					{
						gameObject->SetPosition(NCLVector3(gameObject->GetPosition().x, gameObject->GetPosition().y + (gameObject->stats.defaultScale.y * multiplier * .5f), gameObject->GetPosition().z));
						gameObject->SetScale(gameObject->stats.defaultScale * multiplier);
						break;
					}
					case SPEED_POWERUP:
					{
						gameObject->stats.defaultInvMass *= multiplier;
						break;
					}
					case METEOR_POWERUP:
					{
						for (int i = 0; i < gameObject->stats.meteors; ++i)
						{
							GameObject* meteor = static_cast<GameObject*>(
								PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(gameObject->GetName() + "Meteor" + std::to_string(i)));

							std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
							std::uniform_int_distribution<int> uni(-6, 6); // guaranteed unbiased
							auto random_integer1 = uni(rng);
							auto random_integer2 = uni(rng);

							meteor->SetPosition(gameObject->GetPosition() + NCLVector3((float)random_integer1 * 10.f, 100.f + (i * 40.f), (float)random_integer2 * 10.f));
							meteor->SetEnabled(true);

						}
						break;
					}
					default:
						break;
				}

				
				gameObject->stats.executeAfter = [gameObject, powerup]()
				{
					gameObject->stats.defaultInvMass = 1.f;
					gameObject->SetScale(gameObject->stats.defaultScale);
					gameObject->stats.executeAfter = std::function<void()>();
					powerup->SetEnabled(true);
				};
			}
		};
	} });

	builders.insert({ "RandomNetworkedPowerUp", [](Node* node)
	{

		GameObject* gameObject = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[0]->value));
		float multiplier = stof(node->children[1]->value);
		GameObject* powerup = static_cast<GameObject*>(
			PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(node->children[2]->value));
		float duration = stof(node->children[3]->value) * 1000;

		return [gameObject, multiplier, powerup, duration]()
		{
			if (!gameObject->stats.executeAfter)
			{
				powerup->SetEnabled(false);
				gameObject->stats.timeToWait = duration;


				switch (PaintGameActionBuilder::r1)
				{
				case SCALE_POWERUP:
				{
					gameObject->SetPosition(NCLVector3(gameObject->GetPosition().x, gameObject->GetPosition().y + (gameObject->stats.defaultScale.y * multiplier * .5f), gameObject->GetPosition().z));
					gameObject->SetScale(gameObject->stats.defaultScale * multiplier);
					break;
				}
				case SPEED_POWERUP:
				{
					gameObject->stats.defaultInvMass *= multiplier;
					break;
				}
				case METEOR_POWERUP:
				{

					for (int i = 0; i < gameObject->stats.meteors; ++i)
					{
						GameObject* meteor = static_cast<GameObject*>(
							PaintGameActionBuilder::database->GetTable("GameObjects")->GetResource(gameObject->GetName() + "Meteor" + std::to_string(i)));

						meteor->SetPosition(gameObject->GetPosition() + NCLVector3((float)PaintGameActionBuilder::others[i] * 10.f, 100.f + (i * 40.f), (float)PaintGameActionBuilder::others[9-i] * 10.f));
						meteor->SetEnabled(true);

					}
					gameObject->stats.timeToWait = 2000.f;
					break;
				}
				default:
					break;
				}


				gameObject->stats.executeAfter = [gameObject, powerup]()
				{
					gameObject->stats.defaultInvMass = 1.f;
					gameObject->SetScale(gameObject->stats.defaultScale);
					gameObject->stats.executeAfter = std::function<void()>();
					powerup->SetEnabled(true);
				};

				if (PaintGameActionBuilder::localPlayer == gameObject->GetName()
					&& PaintGameActionBuilder::online)
				{
					DeliverySystem::GetPostman()->InsertMessage(TextMessage("NetworkClient", "collision " + gameObject->GetName() + " " + powerup->GetName()));
				}
			}
		};
	} });
}

Executable PaintGameActionBuilder::BuildExecutable(Node* node)
{
	return builders.at(node->nodeType)(node);
}

void PaintGameActionBuilder::UpdateBufferedVariables()
{
	if (PaintGameActionBuilder::online)
	{
		PaintGameActionBuilder::r1 = PaintGameActionBuilder::r1ToSet;

		for (int i = 0; i < 10; ++i)
		{
			PaintGameActionBuilder::others[i] = PaintGameActionBuilder::othersToSet[i];
		}
	}
}
