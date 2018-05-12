#include "XMLWriter.h"

#include "FilePaths.h"
#include "Database/Database.h"
#include "../Physics/PhysicsNode.h"
#include "../Extrernal Libs/rapidxml-1.13/rapidxml_print.hpp"
#include "../Gameplay/GameplaySystem.h"

#include <fstream>
#include <array>
#include "../Audio/Sound.h"
#include "../Graphics/Utility/Light.h"

XMLWriter::XMLWriter(Database* database, GameplaySystem* gameplay)
{
	this->database = database;
	this->gameplay = gameplay;
}

XMLWriter::~XMLWriter()
{
}

void XMLWriter::saveLevelFile(std::string levelName)
{
	createFolder((LEVELDIR + levelName).c_str());

	rapidxml::xml_document<> levelFile;
	rapidxml::xml_node<>* root = levelFile.allocate_node(rapidxml::node_element, "Level");
	levelFile.append_node(root);

	std::string s = levelName + "/Meshes.xml";
	saveMeshFile(s);
	rapidxml::xml_node<>* fileNode = levelFile.allocate_node(rapidxml::node_element, "File", s.c_str());
	root->append_node(fileNode);

	std::string s1 = levelName + "/Details.xml";
	saveLevelDetails(s1);
	rapidxml::xml_node<>* fileNode1 = levelFile.allocate_node(rapidxml::node_element, "File", s1.c_str());
	root->append_node(fileNode1);

	std::string s2 = levelName + "/Sounds.xml";
	saveSoundsFile(s2);
	rapidxml::xml_node<>* fileNode2 = levelFile.allocate_node(rapidxml::node_element, "File", s2.c_str());
	root->append_node(fileNode2);

	std::string s3 = levelName + "/Lights.xml";
	saveLightsFile(s3);
	rapidxml::xml_node<>* fileNode3 = levelFile.allocate_node(rapidxml::node_element, "File", s3.c_str());
	root->append_node(fileNode3);

	rapidxml::xml_node<>* UI = levelFile.allocate_node(rapidxml::node_element, "UI");
	root->append_node(UI);
	rapidxml::xml_node<>* uiFile = levelFile.allocate_node(rapidxml::node_element, "File", "../Data/UserInterface/InGameMenu.xml");
	UI->append_node(uiFile);

	std::string gameplayFile = gameplay->getGameplayFile();

	if (gameplayFile != "")
	{
		rapidxml::xml_node<>* gameplayNode = levelFile.allocate_node(rapidxml::node_element, "GamePlay");
		root->append_node(gameplayNode);

		rapidxml::xml_node<>* gameplayFileNode = levelFile.allocate_node(rapidxml::node_element, "File", gameplayFile.c_str());
		gameplayNode->append_node(gameplayFileNode);
	}

	std::vector<GameObjectLogic>* objectLogics = gameplay->getGameObjectLogics();

	if (objectLogics->size() > 0)
	{
		rapidxml::xml_node<>* gameLogicNode = levelFile.allocate_node(rapidxml::node_element, "GameLogic");
		root->append_node(gameLogicNode);
		std::vector<std::string> objectLogicFiles;

		for (size_t i = 0; i < objectLogics->size(); ++i)
		{
			objectLogicFiles.push_back((*objectLogics)[i].getScriptFile());
			rapidxml::xml_node<>* gameLogicFileNode = levelFile.allocate_node(rapidxml::node_element, "File", objectLogicFiles[i].c_str());
			gameLogicNode->append_node(gameLogicFileNode);
		}
	}

	std::ofstream file_stored((LEVELDIR + levelName + ".xml").c_str());
	file_stored << levelFile;
	file_stored.close();
	levelFile.clear();
}

void XMLWriter::createFolder(const char* path)
{
	CreateDirectory(path, NULL);
}

void XMLWriter::saveMeshFile(std::string meshFileName)
{
	rapidxml::xml_document<> meshFile;
	rapidxml::xml_node<>* root = meshFile.allocate_node(rapidxml::node_element, "LevelMeshes");
	meshFile.append_node(root);

	auto meshes = database->getTable("Meshes")->getAllResources()->getResourceBuffer();

	std::vector<std::string> meshNames;
	std::vector<std::string> meshFileNames;
	std::vector<std::string> meshTextures;

	for (auto meshIterator = meshes.begin(); meshIterator != meshes.end(); ++meshIterator)
	{
		meshNames.push_back((*meshIterator).second->getName());

		Mesh* mesh = static_cast<Mesh*>((*meshIterator).second);
		meshFileNames.push_back(mesh->file);

		if (mesh->hasTexture)
		{
			meshTextures.push_back(mesh->textureFile);
		}
		else
		{
			meshTextures.push_back("");
		}
	}

	for (size_t i = 0; i < meshNames.size(); ++i)
	{
		rapidxml::xml_node<>* meshNode = meshFile.allocate_node(rapidxml::node_element, "Meshes");
		meshNode->append_attribute(meshFile.allocate_attribute("name", meshNames[i].c_str()));
		root->append_node(meshNode);

		rapidxml::xml_node<>* meshFileNode = meshFile.allocate_node(rapidxml::node_element, "MeshFile", meshFileNames[i].c_str());
		meshNode->append_node(meshFileNode);

		if (meshTextures[i] != "")
		{
			rapidxml::xml_node<>* textureFileNode = meshFile.allocate_node(rapidxml::node_element, "texture", meshTextures[i].c_str());
			meshNode->append_node(textureFileNode);
		}
	}

	std::ofstream file_stored((LEVELDIR + meshFileName).c_str());
	file_stored << meshFile;
	file_stored.close();
	meshFile.clear();
}

void XMLWriter::saveSoundsFile(std::string soundFileName)
{
	rapidxml::xml_document<> soundFile;
	rapidxml::xml_node<>* root = soundFile.allocate_node(rapidxml::node_element, "LevelSounds");
	soundFile.append_node(root);

	auto sounds = database->getTable("SoundObjects")->getAllResources()->getResourceBuffer();

	std::vector<std::string> soundObjectNames;
	std::vector<std::string> soundFiles;

	for (auto soundIterator = sounds.begin(); soundIterator != sounds.end(); ++soundIterator)
	{
		soundObjectNames.push_back((*soundIterator).second->getName());

		Sound* sound = static_cast<Sound*>((*soundIterator).second);
		soundFiles.push_back(sound->getSoundFile());
	}

	for (size_t i = 0; i < soundObjectNames.size(); ++i)
	{
		rapidxml::xml_node<>* soundNode = soundFile.allocate_node(rapidxml::node_element, "SoundObjects", soundFiles[i].c_str());
		soundNode->append_attribute(soundFile.allocate_attribute("name", soundObjectNames[i].c_str()));
		root->append_node(soundNode);
	}

	std::ofstream file_stored((LEVELDIR + soundFileName).c_str());
	file_stored << soundFile;
	file_stored.close();
	soundFile.clear();
}

void XMLWriter::saveLightsFile(std::string lightsFileName)
{
	const LevelLightsState state = getLightsState();

	rapidxml::xml_document<> levelLights;
	rapidxml::xml_node<>* root = levelLights.allocate_node(rapidxml::node_element, "LevelLights");
	levelLights.append_node(root);


	for (size_t i = 0; i < state.lightNames.size(); ++i)
	{
		rapidxml::xml_node<>* lightNode = levelLights.allocate_node(rapidxml::node_element, "Lights");
		lightNode->append_attribute(levelLights.allocate_attribute("name", state.lightNames[i].c_str()));
		root->append_node(lightNode);

		rapidxml::xml_node<>* positionNode = levelLights.allocate_node(rapidxml::node_element, "Position");
		lightNode->append_node(positionNode);
		rapidxml::xml_node<>* positionXNode = levelLights.allocate_node(rapidxml::node_element, "x", state.lightPositions[i][0].c_str());
		positionNode->append_node(positionXNode);
		rapidxml::xml_node<>* positionYNode = levelLights.allocate_node(rapidxml::node_element, "y", state.lightPositions[i][1].c_str());
		positionNode->append_node(positionYNode);
		rapidxml::xml_node<>* positionZNode = levelLights.allocate_node(rapidxml::node_element, "z", state.lightPositions[i][2].c_str());
		positionNode->append_node(positionZNode);

		rapidxml::xml_node<>* colourNode = levelLights.allocate_node(rapidxml::node_element, "colour");
		lightNode->append_node(colourNode);
		rapidxml::xml_node<>* rNode = levelLights.allocate_node(rapidxml::node_element, "r", state.lightColours[i][0].c_str());
		colourNode->append_node(rNode);
		rapidxml::xml_node<>* gNode = levelLights.allocate_node(rapidxml::node_element, "g", state.lightColours[i][1].c_str());
		colourNode->append_node(gNode);
		rapidxml::xml_node<>* bNode = levelLights.allocate_node(rapidxml::node_element, "b", state.lightColours[i][2].c_str());
		colourNode->append_node(bNode);
		rapidxml::xml_node<>* aNode = levelLights.allocate_node(rapidxml::node_element, "a", state.lightColours[i][3].c_str());
		colourNode->append_node(aNode);

		rapidxml::xml_node<>* radiusNode = levelLights.allocate_node(rapidxml::node_element, "radius", state.radii[i].c_str());
		lightNode->append_node(radiusNode);

		rapidxml::xml_node<>* intensityNode = levelLights.allocate_node(rapidxml::node_element, "intensity", state.intensities[i].c_str());
		lightNode->append_node(intensityNode);

		rapidxml::xml_node<>* shadowCastingNode = levelLights.allocate_node(rapidxml::node_element, "shadowcasting", state.shadowcasting[i].c_str());
		lightNode->append_node(shadowCastingNode);
	}

	std::ofstream file_stored((LEVELDIR + lightsFileName).c_str());
	file_stored << levelLights;
	file_stored.close();
	levelLights.clear();
}

void XMLWriter::saveLevelDetails(std::string levelDetailsFile)
{
	const LevelGameObjectsState state = getGameObjectStates();

	rapidxml::xml_document<> levelDetails;
	rapidxml::xml_node<>* root = levelDetails.allocate_node(rapidxml::node_element, "LevelDetails");
	levelDetails.append_node(root);

	int physicsNodeIndex = 0;
	for (size_t i = 0; i < state.hasPhysicsNode.size(); ++i)
	{
		rapidxml::xml_node<>* gameObjectNode = levelDetails.allocate_node(rapidxml::node_element, "GameObjects");
		gameObjectNode->append_attribute(levelDetails.allocate_attribute("name", state.gameObjectNames[i].c_str()));
		root->append_node(gameObjectNode);

		rapidxml::xml_node<>* sceneNodeNode = levelDetails.allocate_node(rapidxml::node_element, "SceneNode");
		gameObjectNode->append_node(sceneNodeNode);

		rapidxml::xml_node<>* meshNode = levelDetails.allocate_node(rapidxml::node_element, "Mesh", state.gameObjectMeshNames[i].c_str());
		sceneNodeNode->append_node(meshNode);

		rapidxml::xml_node<>* colourNode = levelDetails.allocate_node(rapidxml::node_element, "Colour");
		sceneNodeNode->append_node(colourNode);

		rapidxml::xml_node<>* rNode = levelDetails.allocate_node(rapidxml::node_element, "R", state.gameObjectColours[i][0].c_str());
		colourNode->append_node(rNode);
		rapidxml::xml_node<>* gNode = levelDetails.allocate_node(rapidxml::node_element, "G", state.gameObjectColours[i][1].c_str());
		colourNode->append_node(gNode);
		rapidxml::xml_node<>* bNode = levelDetails.allocate_node(rapidxml::node_element, "B", state.gameObjectColours[i][2].c_str());
		colourNode->append_node(bNode);
		rapidxml::xml_node<>* aNode = levelDetails.allocate_node(rapidxml::node_element, "A", state.gameObjectColours[i][3].c_str());
		colourNode->append_node(aNode);

		rapidxml::xml_node<>* enabledNode = levelDetails.allocate_node(rapidxml::node_element, "Enabled", "True/False");
		gameObjectNode->append_node(enabledNode);

		rapidxml::xml_node<>* positionNode = levelDetails.allocate_node(rapidxml::node_element, "Position");
		gameObjectNode->append_node(positionNode);
		rapidxml::xml_node<>* positionXNode = levelDetails.allocate_node(rapidxml::node_element, "X", state.gameObjectPositions[i][0].c_str());
		positionNode->append_node(positionXNode);
		rapidxml::xml_node<>* positionYNode = levelDetails.allocate_node(rapidxml::node_element, "Y", state.gameObjectPositions[i][1].c_str());
		positionNode->append_node(positionYNode);
		rapidxml::xml_node<>* positionZNode = levelDetails.allocate_node(rapidxml::node_element, "Z", state.gameObjectPositions[i][2].c_str());
		positionNode->append_node(positionZNode);

		rapidxml::xml_node<>* rotationNode = levelDetails.allocate_node(rapidxml::node_element, "Rotation");
		gameObjectNode->append_node(rotationNode);
		rapidxml::xml_node<>* rotationXNode = levelDetails.allocate_node(rapidxml::node_element, "X", state.gameObjectRotations[i][0].c_str());
		rotationNode->append_node(rotationXNode);
		rapidxml::xml_node<>* rotationYNode = levelDetails.allocate_node(rapidxml::node_element, "Y", state.gameObjectRotations[i][1].c_str());
		rotationNode->append_node(rotationYNode);
		rapidxml::xml_node<>* rotationZNode = levelDetails.allocate_node(rapidxml::node_element, "Z", state.gameObjectRotations[i][2].c_str());
		rotationNode->append_node(rotationZNode);
		rapidxml::xml_node<>* rotationWNode = levelDetails.allocate_node(rapidxml::node_element, "W", state.gameObjectRotations[i][3].c_str());
		rotationNode->append_node(rotationWNode);

		rapidxml::xml_node<>* scaleNode = levelDetails.allocate_node(rapidxml::node_element, "Scale");
		gameObjectNode->append_node(scaleNode);
		rapidxml::xml_node<>* scaleXNode = levelDetails.allocate_node(rapidxml::node_element, "X", state.gameObjectScales[i][0].c_str());
		scaleNode->append_node(scaleXNode);
		rapidxml::xml_node<>* scaleYNode = levelDetails.allocate_node(rapidxml::node_element, "Y", state.gameObjectScales[i][1].c_str());
		scaleNode->append_node(scaleYNode);
		rapidxml::xml_node<>* scaleZNode = levelDetails.allocate_node(rapidxml::node_element, "Z", state.gameObjectScales[i][2].c_str());
		scaleNode->append_node(scaleZNode);

		if (state.hasPhysicsNode[i])
		{
			rapidxml::xml_node<>* physicsNodeNode = levelDetails.allocate_node(rapidxml::node_element, "PhysicsNode");
			gameObjectNode->append_node(physicsNodeNode);

			rapidxml::xml_node<>* physicsNodeEnabledNode = levelDetails.allocate_node(rapidxml::node_element, "Enabled", 
				state.physicsNodeEnabled[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(physicsNodeEnabledNode);

			rapidxml::xml_node<>* transmitCollisionNode = levelDetails.allocate_node(rapidxml::node_element, "TransmitCollision", 
				state.physicsNodeTransmitCollision[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(transmitCollisionNode);

			rapidxml::xml_node<>* collisionShapeNode = levelDetails.allocate_node(rapidxml::node_element, "CollisionShape", 
				state.collisionShapes[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(collisionShapeNode);

			rapidxml::xml_node<>* massNode = levelDetails.allocate_node(rapidxml::node_element, "Mass",
				state.mass[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(massNode);

			rapidxml::xml_node<>* isCollisionNode = levelDetails.allocate_node(rapidxml::node_element, "isCollision",
				state.isCollision[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(isCollisionNode);

			rapidxml::xml_node<>* elasticityNode = levelDetails.allocate_node(rapidxml::node_element, "Elasticity",
				state.elasticity[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(elasticityNode);

			rapidxml::xml_node<>* frictionNode = levelDetails.allocate_node(rapidxml::node_element, "Friction",
				state.friction[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(frictionNode);

			rapidxml::xml_node<>* dampingNode = levelDetails.allocate_node(rapidxml::node_element, "Damping",
				state.damping[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(dampingNode);

			rapidxml::xml_node<>* isStaticNode = levelDetails.allocate_node(rapidxml::node_element, "isStatic",
				state.isStatic[physicsNodeIndex].c_str());
			physicsNodeNode->append_node(isStaticNode);

			++physicsNodeIndex;
		}
	}

	std::ofstream file_stored((LEVELDIR + levelDetailsFile).c_str());
	file_stored << levelDetails;
	file_stored.close();
	levelDetails.clear();
}

LevelGameObjectsState XMLWriter::getGameObjectStates()
{
	auto gameObjects = database->getTable("GameObjects")->getAllResources()->getResourceBuffer();

	LevelGameObjectsState state;

	for (auto gameObjectIterator = gameObjects.begin(); gameObjectIterator != gameObjects.end(); ++gameObjectIterator)
	{
		GameObject* gameObject = static_cast<GameObject*>((*gameObjectIterator).second);

		state.gameObjectNames.push_back((*gameObjectIterator).first);
		state.gameObjectMeshNames.push_back(gameObject->getSceneNode()->GetMesh()->getName());

		state.gameObjectColours.push_back(std::array<std::string, 4>
		{
			std::to_string(gameObject->getSceneNode()->getColour().x),
			std::to_string(gameObject->getSceneNode()->getColour().y),
			std::to_string(gameObject->getSceneNode()->getColour().z),
			std::to_string(gameObject->getSceneNode()->getColour().w)
		});

		state.gameObjectScales.push_back(std::array<std::string, 3>
		{
			std::to_string(gameObject->getScale().x),
			std::to_string(gameObject->getScale().y),
			std::to_string(gameObject->getScale().z)
		});

		if (gameObject->getPhysicsNode() != nullptr)
		{
			PhysicsNode* physicsNode = gameObject->getPhysicsNode();

			state.gameObjectPositions.push_back(std::array<std::string, 3>
			{
				std::to_string(physicsNode->getPosition().x),
				std::to_string(physicsNode->getPosition().y),
				std::to_string(physicsNode->getPosition().z)
			});

			NCLVector4 rotation = Quaternion::quaternionToAxisAngle(physicsNode->getOrientation());

			state.gameObjectRotations.push_back(std::array<std::string, 4>
			{
				std::to_string(rotation.x),
				std::to_string(rotation.y),
				std::to_string(rotation.z),
				std::to_string(rotation.w)
			});

			state.hasPhysicsNode.push_back(true);

			if (physicsNode->getEnabled())
			{
				state.physicsNodeEnabled.push_back("True");
			}
			else
			{
				state.physicsNodeEnabled.push_back("False");
			}

			if (physicsNode->transmitCollision)
			{
				state.physicsNodeTransmitCollision.push_back("True");
			}
			else
			{
				state.physicsNodeTransmitCollision.push_back("False");
			}

			state.collisionShapes.push_back(physicsNode->collisionShapeType);
			state.mass.push_back(std::to_string(physicsNode->getInverseMass()));

			if (physicsNode->getIsCollision())
			{
				state.isCollision.push_back("True");
			}
			else
			{
				state.isCollision.push_back("False");
			}

			state.elasticity.push_back(std::to_string(physicsNode->getElasticity()));

			state.friction.push_back(std::to_string(physicsNode->getFriction()));
			state.damping.push_back(std::to_string(physicsNode->getDamping()));

			if (physicsNode->getIsStatic())
			{
				state.isStatic.push_back("True");
			}
			else
			{
				state.isStatic.push_back("False");
			}
		}
		else
		{
			state.gameObjectPositions.push_back(std::array<std::string, 3>
			{
				std::to_string(gameObject->getSceneNode()->GetTransform().getPositionVector().x),
				std::to_string(gameObject->getSceneNode()->GetTransform().getPositionVector().y),
				std::to_string(gameObject->getSceneNode()->GetTransform().getPositionVector().z)
			});

			state.gameObjectRotations.push_back(std::array<std::string, 4>
			{
				std::to_string(gameObject->getSceneNode()->axisAngleRotation.x),
				std::to_string(gameObject->getSceneNode()->axisAngleRotation.y),
				std::to_string(gameObject->getSceneNode()->axisAngleRotation.z),
				std::to_string(gameObject->getSceneNode()->axisAngleRotation.w)
			});

			state.hasPhysicsNode.push_back(false);
		}
	}

	return state;
}

LevelLightsState XMLWriter::getLightsState()
{
	auto lights = database->getTable("Lights")->getAllResources()->getResourceBuffer();

	LevelLightsState state;

	for (auto lightsIterator = lights.begin(); lightsIterator != lights.end(); ++lightsIterator)
	{
		Light* light = static_cast<Light*>((*lightsIterator).second);

		state.lightNames.push_back(light->getName());

		state.lightPositions.push_back(std::array<std::string, 3>
		{
			std::to_string(light->GetPosition().x),
			std::to_string(light->GetPosition().y),
			std::to_string(light->GetPosition().z)
		});

		state.lightColours.push_back(std::array<std::string, 4>
		{
			std::to_string(light->GetColour().x),
			std::to_string(light->GetColour().y),
			std::to_string(light->GetColour().z),
			std::to_string(light->GetColour().w)
		});

		state.radii.push_back(to_string(light->GetRadius()));
		state.intensities.push_back(to_string(light->GetData().intensity));

		if (light->IsShadowCasting())
		{
			state.shadowcasting.push_back("enabled");
		}
		else
		{
			state.shadowcasting.push_back("disabled");
		}
	}

	return state;
}
