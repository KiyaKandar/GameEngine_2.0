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

void RenderingSystem::Initialise(Database* database)
{

	std::vector<MessageType> types = { MessageType::TEXT, MessageType::TEXT_MESH_MESSAGE, MessageType::RELATIVE_TRANSFORM,
		MessageType::TOGGLE_GRAPHICS_MODULE, MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, MessageType::PREPARE_PAINT_SURFACE,
		MessageType::SCALE_GAMEOBJECT, MessageType::PAINT_TRAIL_FOR_GAMEOBJECT, MessageType::ADD_SCORE_HOLDER,
		MessageType::ABSOLUTE_TRANSFORM, MessageType::MOVE_GAMEOBJECT, MessageType::ROTATE_GAMEOBJECT,
		MessageType::TOGGLE_GAMEOBJECT, MessageType::DEBUG_LINE, MessageType::DEBUG_SPHERE};

	incomingMessages = MessageProcessor(types, DeliverySystem::GetPostman()->GetDeliveryPoint("RenderingSystem"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::DEBUG_LINE, [&renderer = renderer](Message* message)
	{
		DebugLineMessage* debugLineMessage = static_cast<DebugLineMessage*>(message);
		static_cast<Wireframe*>(renderer->GetGraphicsModule("Wireframe"))->AddLine(debugLineMessage->from, debugLineMessage->to, debugLineMessage->colour);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::DEBUG_SPHERE, [&renderer = renderer](Message* message)
	{
		DebugSphereMessage* debugCircleMessage = static_cast<DebugSphereMessage*>(message);
		static_cast<Wireframe*>(renderer->GetGraphicsModule("Wireframe"))->AddSphere(debugCircleMessage->position, debugCircleMessage->radius, debugCircleMessage->colour);
	});


	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [&renderer = renderer, database = database, &blockCamera = blockCamera](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addscenenode")
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(tokens[1]));

			renderer->AddSceneNode(gameObject->GetSceneNode());
		}
		else if (tokens[0] == "removescenenode")
		{
			renderer->RemoveSceneNodeByResourceName(tokens[1]);
		}
		else if (tokens[0] == "addlight")
		{
			Light* light = static_cast<Light*>(database->GetTable("Lights")->GetResource(tokens[1]));
			(*renderer->GetSceneManager()->GetAllLights())->push_back(light);
		}
		else if (tokens[0] == "removelight")
		{
			for (auto lightIterator = (*renderer->GetSceneManager()->GetAllLights())->begin();
				lightIterator != (*renderer->GetSceneManager()->GetAllLights())->end(); ++lightIterator)
			{
				if ((*lightIterator)->GetName() == tokens[1])
				{
					(*renderer->GetSceneManager()->GetAllLights())->erase(lightIterator);
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
				database->GetTable("GameObjects")->GetResource(tokens[1]));

			gameObject->GetSceneNode()->GetMesh()->SetupMesh();
		}
		else if (tokens[0] == "setupmesh")
		{
			Mesh* mesh = static_cast<Mesh*>(
				database->GetTable("Meshes")->GetResource(tokens[1]));

			mesh->SetupMesh();
		}
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::MOVE_GAMEOBJECT, [database = database](Message* message)
	{
		MoveGameObjectMessage* moveMessage = static_cast<MoveGameObjectMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(moveMessage->gameObjectID));

		gameObject->GetSceneNode()->SetPosition(moveMessage->position);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::SCALE_GAMEOBJECT, [database = database](Message* message)
	{
		ScaleGameObjectMessage* scaleMessage = static_cast<ScaleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(scaleMessage->gameObjectID));

		gameObject->SetScale(scaleMessage->scale);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::ROTATE_GAMEOBJECT, [database = database](Message* message)
	{
		RotateGameObjectMessage* rotateMessage = static_cast<RotateGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(rotateMessage->gameObjectID));

		NCLVector3 position = gameObject->GetSceneNode()->GetTransform().getPositionVector();
		NCLVector3 scale = gameObject->GetSceneNode()->GetTransform().getScalingVector();

		gameObject->GetSceneNode()->axisAngleRotation = rotateMessage->rotation;

		gameObject->GetSceneNode()->SetTransform(NCLMatrix4::translation(position) *
			NCLMatrix4::rotation(rotateMessage->rotation.w, NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z)) *
			NCLMatrix4::scale(scale));
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TOGGLE_GAMEOBJECT, [database = database](Message* message)
	{
		ToggleGameObjectMessage* toggleMessage = static_cast<ToggleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(toggleMessage->gameObjectID));

		gameObject->GetSceneNode()->SetEnabled(toggleMessage->isEnabled);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT_MESH_MESSAGE, [database = database, &renderer = renderer](Message* message)
	{
		TextMeshMessage* textMessage = static_cast<TextMeshMessage*>(message);

		static_cast<GameText*>(renderer->GetGraphicsModule("GameText"))->BufferText(
			textMessage->text, textMessage->position, textMessage->scale, textMessage->colour, textMessage->orthographic, textMessage->hasBackground);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::RELATIVE_TRANSFORM, [database = database](Message* message)
	{
		RelativeTransformMessage* translationMessage = static_cast<RelativeTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(translationMessage->resourceName));

		gameObject->GetSceneNode()->SetTransform(gameObject->GetSceneNode()->GetTransform()
			* translationMessage->transform);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::ABSOLUTE_TRANSFORM, [database = database](Message* message)
	{
		AbsoluteTransformMessage* translationMessage = static_cast<AbsoluteTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(translationMessage->resourceName));

		gameObject->GetSceneNode()->SetTransform(translationMessage->transform);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TOGGLE_GRAPHICS_MODULE, [&renderer = renderer](Message* message)
	{
		ToggleGraphicsModuleMessage* moduleMessage = static_cast<ToggleGraphicsModuleMessage*>(message);
		renderer->ToggleModule(moduleMessage->moduleName, moduleMessage->enabled);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, [&camera = camera, database = database, &blockCamera = blockCamera](Message* message)
	{
		if (!blockCamera)
		{
			MoveCameraRelativeToGameObjectMessage* movementMessage = static_cast<MoveCameraRelativeToGameObjectMessage*>(message);

			GameObject* gameObject = static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(movementMessage->resourceName));

			camera->SetPosition(gameObject->GetSceneNode()->GetTransform().getPositionVector() + movementMessage->translation);
			camera->SetPitch(movementMessage->pitch);
			camera->SetYaw(movementMessage->yaw);
		}
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::PREPARE_PAINT_SURFACE, [database = database, &renderer = renderer](Message* message)
	{
		PreparePaintSurfaceMessage* paintMessage = static_cast<PreparePaintSurfaceMessage*>(message);

		std::vector<GameObject*> surfaceObjects;

		for (std::string objectIdentifiers : paintMessage->surfaceObjectIdentifiers)
		{
			surfaceObjects.push_back(static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(objectIdentifiers)));
		}

		static_cast<PaintTrail*>(renderer->GetGraphicsModule("PaintTrail"))->preparePaintSurface(surfaceObjects);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::PAINT_TRAIL_FOR_GAMEOBJECT, [database = database, &renderer = renderer](Message* message)
	{
		PaintTrailForGameObjectMessage* paintMessage = static_cast<PaintTrailForGameObjectMessage*>(message);

		GameObject* painterGameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(paintMessage->resourceName));

		static_cast<PaintTrail*>(renderer->GetGraphicsModule("PaintTrail"))->addPainterObjectForNextFrame(painterGameObject);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::ADD_SCORE_HOLDER, [&renderer = renderer](Message* message)
	{
		AddScoreHolderMessage* scoreMessage = static_cast<AddScoreHolderMessage*>(message);

		static_cast<ScoreCounter*>(renderer->GetGraphicsModule("ScoreCounter"))->bufferScoreHolder(scoreMessage->name);
	});
}

void RenderingSystem::RenderLoadingScreen(const float& deltaTime)
{
	renderer->RenderLoadingScreen(deltaTime);
}

void RenderingSystem::SetupMeshes()
{
	std::vector<SceneNode*>** nodes = scene->GetAllNodes();

	for (SceneNode* node : **nodes)
	{
		node->GetMesh()->SetupMesh();
	}
}

void RenderingSystem::SetSceneToRender(SceneManager* scene, Database* database)
{
	this->scene = scene;
	renderer->Initialise(scene, database);
}

void RenderingSystem::UpdateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();
	renderer->Update(deltaTime);
	timer->endTimedSection();
}