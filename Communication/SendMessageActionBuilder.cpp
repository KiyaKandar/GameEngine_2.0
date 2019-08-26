#include "SendMessageActionBuilder.h"

#include "Messages/TextMessage.h"

#include <iterator>
#include <sstream>
#include "Messages/AbsoluteTransformMessage.h"
#include "Messages/MoveGameObjectMessage.h"
#include "Messages/ScaleGameObjectMessage.h"
#include "Messages/ToggleGameObjectMessage.h"
#include "LetterBox.h"

std::unordered_map<std::string, Builder>SendMessageActionBuilder::builders 
	= std::unordered_map<std::string, Builder>();

std::unordered_map<std::string, DevConsoleNodeBuilder>SendMessageActionBuilder::devConsoleBuilder
	= std::unordered_map<std::string, DevConsoleNodeBuilder>();

void SendMessageActionBuilder::InitialiseBuilders()
{
	InitialiseNodeBuilders();
	InitialiseDevConsoleBuilders();
}

Executable SendMessageActionBuilder::BuildSendMessageAction(Node* node)
{
	return builders.at(node->name)(node);
}

Executable SendMessageActionBuilder::BuildSendMessageAction(std::string devConsoleLine)
{
	istringstream iss(devConsoleLine);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	return devConsoleBuilder.at(tokens[0])(tokens);
}

void SendMessageActionBuilder::InitialiseNodeBuilders()
{
	builders.insert({ "TEXT" , [](Node* node)
	{
		Node* destination = node->children[0];
		Node* data = node->children[1];

		return[destination = destination->value, text = data->value]()
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage(destination, text));
		};
	} });

	builders.insert({ "RELATIVE_TRANSFORM" , [](Node* node)
	{
		return BuildExecutable<RelativeTransformMessage>(node, &LetterBox::ADD_SENDER_RelativeTransformMessage);
	} });

	builders.insert({ "ABSOLUTE_TRANSFORM" , [](Node* node)
	{
		return BuildExecutable<AbsoluteTransformMessage>(node, &LetterBox::ADD_SENDER_AbsoluteTransformMessage);
	} });

	builders.insert({ "MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<MoveCameraRelativeToGameObjectMessage>(node, &LetterBox::ADD_SENDER_MoveCameraRelativeToGameObjectMessage);
	} });

	builders.insert({ "TOGGLE_GRAPHICS_MODULE" , [](Node* node)
	{
		return BuildExecutable<ToggleGraphicsModuleMessage>(node, &LetterBox::ADD_SENDER_ToggleGraphicsModuleMessage);
	} });

	builders.insert({ "PREPARE_PAINT_SURFACE" , [](Node* node)
	{
		return BuildExecutable<PreparePaintSurfaceMessage>(node, &LetterBox::ADD_SENDER_PreparePaintSurfaceMessage);
	} });

	builders.insert({ "PAINT_TRAIL_FOR_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<PaintTrailForGameObjectMessage>(node, &LetterBox::ADD_SENDER_PaintTrailForGameObjectMessage);
	} });

	builders.insert({ "APPLY_IMPULSE" , [](Node* node)
	{
		return BuildExecutable<ApplyImpulseMessage>(node, &LetterBox::ADD_SENDER_ApplyImpulseMessage);
	} });

	builders.insert({ "APPLY_FORCE" , [](Node* node)
	{
		return BuildExecutable<ApplyForceMessage>(node, &LetterBox::ADD_SENDER_ApplyForceMessage);
	} });

	builders.insert({ "ADD_SCORE_HOLDER" , [](Node* node)
	{
		return BuildExecutable<AddScoreHolderMessage>(node, &LetterBox::ADD_SENDER_AddScoreHolderMessage);
	} });

	builders.insert({ "MOVE_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<MoveGameObjectMessage>(node, &LetterBox::ADD_SENDER_MoveGameObjectMessage);
	} });

	builders.insert({ "SCALE_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<ScaleGameObjectMessage>(node, &LetterBox::ADD_SENDER_ScaleGameObjectMessage);
	} });

	builders.insert({ "ROTATE_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<RotateGameObjectMessage>(node, &LetterBox::ADD_SENDER_RotateGameObjectMessage);
	} });

	builders.insert({ "TOGGLE_GAMEOBJECT" , [](Node* node)
	{
		return BuildExecutable<ToggleGameObjectMessage>(node, &LetterBox::ADD_SENDER_ToggleGameObjectMessage);
	} });

	builders.insert({ "PLAY_ANIMATION" , [](Node* node)
	{
		return BuildExecutable<PlayAnimationMessage>(node, &LetterBox::ADD_SENDER_PlayAnimationMessage);
	} });

	builders.insert({ "PLAY_SOUND" , [](Node* node)
	{
		return BuildExecutable<PlaySoundMessage>(node, &LetterBox::ADD_SENDER_PlaySoundMessage);
	} });

	builders.insert({ "MOVING_SOUND" , [](Node* node)
	{
		return BuildExecutable<PlayMovingSoundMessage>(node, &LetterBox::ADD_SENDER_PlayMovingSoundMessage);
	} });

	builders.insert({ "TEXT_MESH" , [](Node* node) -> Executable
	{
		return BuildExecutable<TextMeshMessage>(node, &LetterBox::ADD_SENDER_TextMeshMessage);
	} });

	builders.insert({ "DUMMY_WORK" , [](Node* node)
		{
			DummyWorkMessage message = DummyWorkMessage::Builder(node);

			return [message = message]() mutable
			{
				message.RandomiseDestination();
				DeliverySystem::GetPostman()->InsertMessage(message);
			};
		} });
}

void SendMessageActionBuilder::InitialiseDevConsoleBuilders()
{
	//../Data/GameObjectLogic/aiObjectLogic.xml
	devConsoleBuilder.insert({ "text" , [](std::vector<std::string> line)
	{
		std::string destination = line[1];

		std::string data;

		for (int i = 2; i < (int)line.size(); ++i)
		{
			data += " " + line[i];
		}

		return[destination = destination, text = data]()
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage(destination, text));
		};
	} });

	devConsoleBuilder.insert({ "relativetransform" , [](std::vector<std::string> line)
	{
		RelativeTransformMessage message = RelativeTransformMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "applyforce" , [](std::vector<std::string> line)
	{
		ApplyForceMessage message = ApplyForceMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "applyimpulse" , [](std::vector<std::string> line)
	{
		ApplyImpulseMessage message = ApplyImpulseMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "addscoreholder" , [](std::vector<std::string> line)
	{
		AddScoreHolderMessage message = AddScoreHolderMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "movegameobject" , [](std::vector<std::string> line)
	{
		MoveGameObjectMessage message = MoveGameObjectMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "scalegameobject" , [](std::vector<std::string> line)
	{
		ScaleGameObjectMessage message = ScaleGameObjectMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "rotategameobject" , [](std::vector<std::string> line)
	{
		RotateGameObjectMessage message = RotateGameObjectMessage::TokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	} });
}
