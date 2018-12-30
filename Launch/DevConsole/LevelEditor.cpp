#include "LevelEditor.h"

#include "GameObject.h"
#include "Resource Management/Database/Database.h"

#include <iterator>
#include <sstream>
#include "Communication/DeliverySystem.h"
#include "Communication/SendMessageActionBuilder.h"
#include "../Physics/PhysicsNode.h"
#include "Resource Management/XMLWriter.h"
#include "../Gameplay/GameplaySystem.h"
#include "FilePaths.h"
#include "Utility/Light.h"

std::map<std::string, LevelEditorAction> LevelEditor::actions
	= std::map<std::string, LevelEditorAction>();
Database* LevelEditor::database = nullptr;
GameplaySystem* LevelEditor::gameplay = nullptr;
std::string LevelEditor::levelFile = "";

void LevelEditor::InitialiseLevelEditor(Database* providedDatabase, GameplaySystem* providedGameplay)
{
	database = providedDatabase;
	gameplay = providedGameplay;
	levelFile = "Level1.xml";

	actions.insert({
		"spawngameobject", [&database = database](std::vector<std::string> devConsoleTokens)
		{
			GameObject* gameObject = new GameObject();
			gameObject->SetSize(sizeof(GameObject));
			gameObject->SetName(devConsoleTokens[1]);

			database->GetTable("GameObjects")->AddNewResource(gameObject);
		}
	});

	actions.insert({
		"addscenenode", [&database = database](std::vector<std::string> devConsoleTokens)
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(devConsoleTokens[1]));

			std::string meshName = devConsoleTokens[2];
			SceneNode* sceneNode = new SceneNode(
				static_cast<Mesh*>(database->GetTable("Meshes")->GetResource(meshName)));

			std::string colourString = devConsoleTokens[3].substr(7);
			NCLVector4 colour = VectorBuilder::buildVector4(colourString);
			sceneNode->SetColour(colour);

			gameObject->SetSceneNode(sceneNode);

			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("RenderingSystem", "addscenenode " + devConsoleTokens[1]));
		}
	});

	actions.insert({
		"addphysicsnode", [&database = database](std::vector<std::string> devConsoleTokens)
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(devConsoleTokens[1]));

			PhysicsNode* physicsnode = new PhysicsNode();
			physicsnode->SetParent(gameObject);
			physicsnode->SetEnabled(true);
			physicsnode->transmitCollision = true;
			physicsnode->SetCollisionShape(devConsoleTokens[2].substr(6));
			physicsnode->SetInverseMass(stof(devConsoleTokens[3].substr(8)));
			physicsnode->SetInverseInertia(
				physicsnode->GetCollisionShape()->BuildInverseInertia(physicsnode->GetInverseMass()));
			physicsnode->SetElasticity(0.5f);
			physicsnode->SetFriction(0.9f);
			physicsnode->SetStatic(devConsoleTokens[4].substr(7) == "true");
			gameObject->SetPhysicsNode(physicsnode);

			DeliverySystem::GetPostman()->
				InsertMessage(TextMessage("Physics", "addphysicsnode " + devConsoleTokens[1]));
		}
	});

	//loadmesh CubeMesh mesh=.../Data/Resources/Meshes/ceneteredcube.obj texture=../../texture
	actions.insert({
		"loadmesh", [&database = database](std::vector<std::string> devConsoleTokens)
		{
			Mesh* mesh = new Mesh(devConsoleTokens[2].substr(5), 1);
			mesh->SetName(devConsoleTokens[1]);

			if (devConsoleTokens.size() == 4)
			{
				//mesh->loadTexture(devConsoleTokens[3].substr(8));
				mesh->SetTextureFile(devConsoleTokens[3].substr(8));
			}

			database->GetTable("Meshes")->AddNewResource(mesh);
			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("RenderingSystem", "setupmesh " + devConsoleTokens[1]));
		}
	});

	//loadsound soundName sound=../Data/...
	actions.insert({
		"loadsounds", [&database = database](std::vector<std::string> devConsoleTokens)
		{
			Sound* sound = new Sound(devConsoleTokens[2].substr(6));
			sound->SetName(devConsoleTokens[1]);

			database->GetTable("SoundObjects")->AddNewResource(sound);
		}
	});

	//addlight lightName position=0,0,0 colour=1,1,1,1 radius=1 intensity=1 shadow=true
	actions.insert({
		"addlight", [](std::vector<std::string> devConsoleTokens)
		{
			std::string positionString = devConsoleTokens[2].substr(9);
			NCLVector3 position = VectorBuilder::buildVector3(positionString);

			std::string colourString = devConsoleTokens[3].substr(7);
			NCLVector4 colour = VectorBuilder::buildVector4(colourString);

			float radius = stof(devConsoleTokens[4].substr(7));
			float intensity = stof(devConsoleTokens[5].substr(10));
			bool shadowCasting = devConsoleTokens[6].substr(7) == "true";

			Light* light = new Light(position, colour, radius, intensity, shadowCasting);
			light->SetName(devConsoleTokens[1]);

			database->GetTable("Lights")->AddNewResource(light);

			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("RenderingSystem", "addlight " + devConsoleTokens[1]));
		}
	});

	actions.insert({
		"removelight", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("RenderingSystem", "removelight " + devConsoleTokens[1]));
		}
	});

	actions.insert({
		"debugcamera", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("Console", "togglecamera"));
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("RenderingSystem", "togglecamera"));
		}
	});

	actions.insert({
		"removescenenode", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("RenderingSystem", "removescenenode " + devConsoleTokens[1]));
		}
	});

	actions.insert({
		"removephysicsnode", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("Physics", "removephysicsnode " + devConsoleTokens[1]));
		}
	});

	actions.insert({
		"savelevel", [](std::vector<std::string> devConsoleTokens)
		{
			LevelEditor::levelFile = devConsoleTokens[1] + ".xml";
			XMLWriter writer(database, LevelEditor::gameplay);
			writer.SaveLevelFile(devConsoleTokens[1]);
		}
	});

	actions.insert({
		"start", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime enable"));
		}
	});

	actions.insert({
		"quit", [](std::vector<std::string> devConsoleTokens)
		{
			DeliverySystem::GetPostman()->InsertMessage(
				TextMessage("GameLoop", "Start False " + LevelEditor::levelFile));
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime disable"));
		}
	});
}

void LevelEditor::ExecuteDevConsoleLine(std::string devConsoleLine)
{
	std::istringstream iss(devConsoleLine);
	std::vector<std::string> tokens{
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{}
	};

	bool executedDevConsoleLine = false;

	for (auto actionIterator = actions.begin(); actionIterator != actions.end(); ++actionIterator)
	{
		if (actionIterator->first == tokens[0])
		{
			actions.at(tokens[0])(tokens);
			executedDevConsoleLine = true;
			break;
		}
	}

	if (!executedDevConsoleLine)
	{
		SendMessageActionBuilder::BuildSendMessageAction(devConsoleLine)();
	}
}
