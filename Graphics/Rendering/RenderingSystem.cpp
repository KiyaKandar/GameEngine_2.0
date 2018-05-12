#include "RenderingSystem.h"

#include "../Communication/Message.h"
#include "../Communication/MessageProcessor.h"
#include "../Communication/DeliverySystem.h"

#include "../Communication/Message.h"

#include <queue>
#include "../../Communication/Messages/PlayerInputMessage.h"
#include "../../Communication/Messages/RelativeTransformMessage.h"
#include "../Resource Management/Database/Database.h"
#include "../../Gameplay/GameObject.h"
#include "../../Communication/Messages/TextMessage.h"
#include "../../Communication/Messages/ToggleGraphicsModuleMessage.h"
#include "../../Communication/Messages/MoveCameraRelativeToGameObjectMessage.h"
#include "../../Communication/Messages/PreparePaintSurfaceMessage.h"
#include "../../Communication/Messages/AddScoreHolderMessage.h"
#include "../../Communication/Messages/ToggleGameObjectMessage.h"
#include "../../Communication/Messages/DebugLineMessage.h"
#include "../Utilities/GameTimer.h"
#include <iterator>
#include "../../Communication/Messages/AbsoluteTransformMessage.h"

RenderingSystem::RenderingSystem(Window* window, Camera* camera)
	: Subsystem("RenderingSystem")
{
	this->camera = camera;
	renderer = std::make_unique<Renderer>(timer, window, camera);
}

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::initialise(Database* database)
{

	std::vector<MessageType> types = { MessageType::TEXT, MessageType::TEXT_MESH_MESSAGE, MessageType::RELATIVE_TRANSFORM,
		MessageType::TOGGLE_GRAPHICS_MODULE, MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, MessageType::PREPARE_PAINT_SURFACE,
		MessageType::SCALE_GAMEOBJECT, MessageType::PAINT_TRAIL_FOR_GAMEOBJECT, MessageType::ADD_SCORE_HOLDER,
		MessageType::ABSOLUTE_TRANSFORM, MessageType::MOVE_GAMEOBJECT, MessageType::ROTATE_GAMEOBJECT,
		MessageType::TOGGLE_GAMEOBJECT, MessageType::DEBUG_LINE, MessageType::DEBUG_SPHERE};

	incomingMessages = MessageProcessor(types, DeliverySystem::getPostman()->getDeliveryPoint("RenderingSystem"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::DEBUG_LINE, [&renderer = renderer](Message* message)
	{
		DebugLineMessage* debugLineMessage = static_cast<DebugLineMessage*>(message);
		static_cast<Wireframe*>(renderer->getGraphicsModule("Wireframe"))->addLine(debugLineMessage->from, debugLineMessage->to, debugLineMessage->colour);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::DEBUG_SPHERE, [&renderer = renderer](Message* message)
	{
		DebugSphereMessage* debugCircleMessage = static_cast<DebugSphereMessage*>(message);
		static_cast<Wireframe*>(renderer->getGraphicsModule("Wireframe"))->addSphere(debugCircleMessage->position, debugCircleMessage->radius, debugCircleMessage->colour);
	});


	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [&renderer = renderer, database = database, &blockCamera = blockCamera](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addscenenode")
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->getTable("GameObjects")->getResource(tokens[1]));

			renderer->addSceneNode(gameObject->getSceneNode());
		}
		else if (tokens[0] == "removescenenode")
		{
			renderer->removeSceneNodeByResourceName(tokens[1]);
		}
		else if (tokens[0] == "addlight")
		{
			Light* light = static_cast<Light*>(database->getTable("Lights")->getResource(tokens[1]));
			(*renderer->getSceneManager()->getAllLights())->push_back(light);
		}
		else if (tokens[0] == "removelight")
		{
			for (auto lightIterator = (*renderer->getSceneManager()->getAllLights())->begin();
				lightIterator != (*renderer->getSceneManager()->getAllLights())->end(); ++lightIterator)
			{
				if ((*lightIterator)->getName() == tokens[1])
				{
					(*renderer->getSceneManager()->getAllLights())->erase(lightIterator);
					break;
				}
			}
		}
		else if (tokens[0] == "togglecamera")
		{
			blockCamera = !blockCamera;
		}
		else if (tokens[0] == "setupmeshgameobject")
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->getTable("GameObjects")->getResource(tokens[1]));

			gameObject->getSceneNode()->GetMesh()->setupMesh();
		}
		else if (tokens[0] == "setupmesh")
		{
			Mesh* mesh = static_cast<Mesh*>(
				database->getTable("Meshes")->getResource(tokens[1]));

			mesh->setupMesh();
		}
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::MOVE_GAMEOBJECT, [database = database](Message* message)
	{
		MoveGameObjectMessage* moveMessage = static_cast<MoveGameObjectMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(moveMessage->gameObjectID));

		gameObject->getSceneNode()->setPosition(moveMessage->position);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::SCALE_GAMEOBJECT, [database = database](Message* message)
	{
		ScaleGameObjectMessage* scaleMessage = static_cast<ScaleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(scaleMessage->gameObjectID));

		gameObject->setScale(scaleMessage->scale);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::ROTATE_GAMEOBJECT, [database = database](Message* message)
	{
		RotateGameObjectMessage* rotateMessage = static_cast<RotateGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(rotateMessage->gameObjectID));

		NCLVector3 position = gameObject->getSceneNode()->GetTransform().getPositionVector();
		NCLVector3 scale = gameObject->getSceneNode()->GetTransform().getScalingVector();

		gameObject->getSceneNode()->axisAngleRotation = rotateMessage->rotation;

		gameObject->getSceneNode()->SetTransform(NCLMatrix4::translation(position) *
			NCLMatrix4::rotation(rotateMessage->rotation.w, NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z)) *
			NCLMatrix4::scale(scale));
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::TOGGLE_GAMEOBJECT, [database = database](Message* message)
	{
		ToggleGameObjectMessage* toggleMessage = static_cast<ToggleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(toggleMessage->gameObjectID));

		gameObject->getSceneNode()->setEnabled(toggleMessage->isEnabled);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT_MESH_MESSAGE, [database = database, &renderer = renderer](Message* message)
	{
		TextMeshMessage* textMessage = static_cast<TextMeshMessage*>(message);

		static_cast<GameText*>(renderer->getGraphicsModule("GameText"))->bufferText(
			textMessage->text, textMessage->position, textMessage->scale, textMessage->colour, textMessage->orthographic, textMessage->hasBackground);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::RELATIVE_TRANSFORM, [database = database](Message* message)
	{
		RelativeTransformMessage* translationMessage = static_cast<RelativeTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(translationMessage->resourceName));

		gameObject->getSceneNode()->SetTransform(gameObject->getSceneNode()->GetTransform()
			* translationMessage->transform);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::ABSOLUTE_TRANSFORM, [database = database](Message* message)
	{
		AbsoluteTransformMessage* translationMessage = static_cast<AbsoluteTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(translationMessage->resourceName));

		gameObject->getSceneNode()->SetTransform(translationMessage->transform);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::TOGGLE_GRAPHICS_MODULE, [&renderer = renderer](Message* message)
	{
		ToggleGraphicsModuleMessage* moduleMessage = static_cast<ToggleGraphicsModuleMessage*>(message);
		renderer->toggleModule(moduleMessage->moduleName, moduleMessage->enabled);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, [&camera = camera, database = database, &blockCamera = blockCamera](Message* message)
	{
		if (!blockCamera)
		{
			MoveCameraRelativeToGameObjectMessage* movementMessage = static_cast<MoveCameraRelativeToGameObjectMessage*>(message);

			GameObject* gameObject = static_cast<GameObject*>(
				database->getTable("GameObjects")->getResource(movementMessage->resourceName));

			camera->setPosition(gameObject->getSceneNode()->GetTransform().getPositionVector() + movementMessage->translation);
			camera->setPitch(movementMessage->pitch);
			camera->setYaw(movementMessage->yaw);
		}
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::PREPARE_PAINT_SURFACE, [database = database, &renderer = renderer](Message* message)
	{
		PreparePaintSurfaceMessage* paintMessage = static_cast<PreparePaintSurfaceMessage*>(message);

		std::vector<GameObject*> surfaceObjects;

		for (std::string objectIdentifiers : paintMessage->surfaceObjectIdentifiers)
		{
			surfaceObjects.push_back(static_cast<GameObject*>(
				database->getTable("GameObjects")->getResource(objectIdentifiers)));
		}

		static_cast<PaintTrail*>(renderer->getGraphicsModule("PaintTrail"))->preparePaintSurface(surfaceObjects);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::PAINT_TRAIL_FOR_GAMEOBJECT, [database = database, &renderer = renderer](Message* message)
	{
		PaintTrailForGameObjectMessage* paintMessage = static_cast<PaintTrailForGameObjectMessage*>(message);

		GameObject* painterGameObject = static_cast<GameObject*>(
			database->getTable("GameObjects")->getResource(paintMessage->resourceName));

		static_cast<PaintTrail*>(renderer->getGraphicsModule("PaintTrail"))->addPainterObjectForNextFrame(painterGameObject);
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::ADD_SCORE_HOLDER, [&renderer = renderer](Message* message)
	{
		AddScoreHolderMessage* scoreMessage = static_cast<AddScoreHolderMessage*>(message);

		static_cast<ScoreCounter*>(renderer->getGraphicsModule("ScoreCounter"))->bufferScoreHolder(scoreMessage->name);
	});
}

void RenderingSystem::renderLoadingScreen(const float& deltaTime)
{
	renderer->renderLoadingScreen(deltaTime);
}

void RenderingSystem::setupMeshes()
{
	std::vector<SceneNode*>** nodes = scene->getAllNodes();

	for (SceneNode* node : **nodes)
	{
		node->GetMesh()->setupMesh();
	}
}

void RenderingSystem::SetSceneToRender(SceneManager* scene, Database* database)
{
	this->scene = scene;
	renderer->initialise(scene, database);
}

void RenderingSystem::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();
	renderer->update(deltaTime);
	timer->endTimedSection();
}