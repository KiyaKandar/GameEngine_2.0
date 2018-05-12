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

void LevelEditor::initialiseLevelEditor(Database* providedDatabase, GameplaySystem* providedGameplay)
{
	database = providedDatabase;
	gameplay = providedGameplay;
	levelFile = "Level1.xml";

	actions.insert({ "spawngameobject", [&database = database](std::vector<std::string> devConsoleTokens)
	{
		GameObject* gameObject = new GameObject();
		gameObject->setSize(sizeof(GameObject));
		gameObject->setName(devConsoleTokens[1]);
		
		database->getTable("GameObjects")->addNewResource(gameObject);
	} });

	actions.insert({ "addscenenode", [&database = database](std::vector<std::string> devConsoleTokens)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(devConsoleTokens[1]));

		std::string meshName = devConsoleTokens[2];
		SceneNode* sceneNode = new SceneNode(static_cast<Mesh*>(database->getTable("Meshes")->getResource(meshName)));

		std::string colourString = devConsoleTokens[3].substr(7);
		NCLVector4 colour = VectorBuilder::buildVector4(colourString);
		sceneNode->SetColour(colour);

		gameObject->setSceneNode(sceneNode);

		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "addscenenode " + devConsoleTokens[1]));
	} });

	actions.insert({ "addphysicsnode", [&database = database](std::vector<std::string> devConsoleTokens)
	{
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(devConsoleTokens[1]));

		PhysicsNode* physicsnode = new PhysicsNode();
		physicsnode->setParent(gameObject);
		physicsnode->setEnabled(true);
		physicsnode->transmitCollision = true;
		physicsnode->setCollisionShape(devConsoleTokens[2].substr(6));
		physicsnode->setInverseMass(stof(devConsoleTokens[3].substr(8)));
		physicsnode->setInverseInertia(physicsnode->getCollisionShape()->buildInverseInertia(physicsnode->getInverseMass()));
		physicsnode->setElasticity(0.5f);
		physicsnode->setFriction(0.9f);
		physicsnode->setStatic(devConsoleTokens[4].substr(7) == "true");
		gameObject->setPhysicsNode(physicsnode);

		DeliverySystem::getPostman()->insertMessage(TextMessage("Physics", "addphysicsnode " + devConsoleTokens[1]));
	} });

	//loadmesh CubeMesh mesh=.../Data/Resources/Meshes/ceneteredcube.obj texture=../../texture
	actions.insert({ "loadmesh", [&database = database](std::vector<std::string> devConsoleTokens)
	{
		Mesh* mesh = new Mesh(devConsoleTokens[2].substr(5), 1);
		mesh->setName(devConsoleTokens[1]);
		
		if (devConsoleTokens.size() == 4)
		{
			//mesh->loadTexture(devConsoleTokens[3].substr(8));
			mesh->setTextureFile(devConsoleTokens[3].substr(8));
		}

		database->getTable("Meshes")->addNewResource(mesh);
		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "setupmesh " + devConsoleTokens[1]));
	} });

	//loadsound soundName sound=../Data/...
	actions.insert({ "loadsounds", [&database = database](std::vector<std::string> devConsoleTokens)
	{
		Sound* sound = new Sound(devConsoleTokens[2].substr(6));
		sound->setName(devConsoleTokens[1]);

		database->getTable("SoundObjects")->addNewResource(sound);
	} });

	//addlight lightName position=0,0,0 colour=1,1,1,1 radius=1 intensity=1 shadow=true
	actions.insert({"addlight", [](std::vector<std::string> devConsoleTokens)
	{
		std::string positionString = devConsoleTokens[2].substr(9);
		NCLVector3 position = VectorBuilder::buildVector3(positionString);

		std::string colourString = devConsoleTokens[3].substr(7);
		NCLVector4 colour = VectorBuilder::buildVector4(colourString);

		float radius = stof(devConsoleTokens[4].substr(7));
		float intensity = stof(devConsoleTokens[5].substr(10));
		bool shadowCasting = devConsoleTokens[6].substr(7) == "true";

		Light* light = new Light(position, colour, radius, intensity, shadowCasting);
		light->setName(devConsoleTokens[1]);

		database->getTable("Lights")->addNewResource(light);

		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "addlight " + devConsoleTokens[1]));
	} });

	actions.insert({ "removelight", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "removelight " + devConsoleTokens[1]));
	} });

	actions.insert({ "debugcamera", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("Console", "togglecamera"));
		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "togglecamera"));
	} });

	actions.insert({ "removescenenode", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "removescenenode " + devConsoleTokens[1]));
	} });

	actions.insert({ "removephysicsnode", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("Physics", "removephysicsnode " + devConsoleTokens[1]));
	} });

	actions.insert({ "savelevel", [](std::vector<std::string> devConsoleTokens)
	{
		LevelEditor::levelFile = devConsoleTokens[1] + ".xml";
		XMLWriter writer(database, LevelEditor::gameplay);
		writer.saveLevelFile(devConsoleTokens[1]);
	} });

	actions.insert({ "start", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime enable"));
	} });

	actions.insert({ "quit", [](std::vector<std::string> devConsoleTokens)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "Start False " + LevelEditor::levelFile));
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime disable"));
	} });
}

void LevelEditor::executeDevConsoleLine(std::string devConsoleLine)
{
	std::istringstream iss(devConsoleLine);
	std::vector<std::string> tokens{
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };

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
		SendMessageActionBuilder::buildSendMessageAction(devConsoleLine)();
	}
}
