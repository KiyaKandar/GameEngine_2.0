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

void SendMessageActionBuilder::initialiseBuilders()
{
	initialiseNodeBuilders();
	initialiseDevConsoleBuilders();
}

Executable SendMessageActionBuilder::buildSendMessageAction(Node* node)
{
	return builders.at(node->name)(node);
}

Executable SendMessageActionBuilder::buildSendMessageAction(std::string devConsoleLine)
{
	istringstream iss(devConsoleLine);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	return devConsoleBuilder.at(tokens[0])(tokens);
}

void SendMessageActionBuilder::initialiseNodeBuilders()
{
	builders.insert({ "TEXT" , [](Node* node)
	{
		Node* destination = node->children[0];
		Node* data = node->children[1];

		return[destination = destination->value, text = data->value]()
		{
			DeliverySystem::getPostman()->insertMessage(TextMessage(destination, text));
		};
	} });

	builders.insert({ "RELATIVE_TRANSFORM" , [](Node* node)
	{
		return buildExecutable<RelativeTransformMessage>(node, &LetterBox::ADD_SENDER_RelativeTransformMessage);
	} });

	builders.insert({ "ABSOLUTE_TRANSFORM" , [](Node* node)
	{
		return buildExecutable<AbsoluteTransformMessage>(node, &LetterBox::ADD_SENDER_AbsoluteTransformMessage);
	} });

	builders.insert({ "MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<MoveCameraRelativeToGameObjectMessage>(node, &LetterBox::ADD_SENDER_MoveCameraRelativeToGameObjectMessage);
	} });

	builders.insert({ "TOGGLE_GRAPHICS_MODULE" , [](Node* node)
	{
		return buildExecutable<ToggleGraphicsModuleMessage>(node, &LetterBox::ADD_SENDER_ToggleGraphicsModuleMessage);
	} });

	builders.insert({ "PREPARE_PAINT_SURFACE" , [](Node* node)
	{
		return buildExecutable<PreparePaintSurfaceMessage>(node, &LetterBox::ADD_SENDER_PreparePaintSurfaceMessage);
	} });

	builders.insert({ "PAINT_TRAIL_FOR_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<PaintTrailForGameObjectMessage>(node, &LetterBox::ADD_SENDER_PaintTrailForGameObjectMessage);
	} });

	builders.insert({ "APPLY_IMPULSE" , [](Node* node)
	{
		return buildExecutable<ApplyImpulseMessage>(node, &LetterBox::ADD_SENDER_ApplyImpulseMessage);
	} });

	builders.insert({ "APPLY_FORCE" , [](Node* node)
	{
		return buildExecutable<ApplyForceMessage>(node, &LetterBox::ADD_SENDER_ApplyForceMessage);
	} });

	builders.insert({ "ADD_SCORE_HOLDER" , [](Node* node)
	{
		return buildExecutable<AddScoreHolderMessage>(node, &LetterBox::ADD_SENDER_AddScoreHolderMessage);
	} });

	builders.insert({ "MOVE_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<MoveGameObjectMessage>(node, &LetterBox::ADD_SENDER_MoveGameObjectMessage);
	} });

	builders.insert({ "SCALE_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<ScaleGameObjectMessage>(node, &LetterBox::ADD_SENDER_ScaleGameObjectMessage);
	} });

	builders.insert({ "ROTATE_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<RotateGameObjectMessage>(node, &LetterBox::ADD_SENDER_RotateGameObjectMessage);
	} });

	builders.insert({ "TOGGLE_GAMEOBJECT" , [](Node* node)
	{
		return buildExecutable<ToggleGameObjectMessage>(node, &LetterBox::ADD_SENDER_ToggleGameObjectMessage);
	} });

	builders.insert({ "PLAY_ANIMATION" , [](Node* node)
	{
		return buildExecutable<PlayAnimationMessage>(node, &LetterBox::ADD_SENDER_PlayAnimationMessage);
	} });

	builders.insert({ "PLAY_SOUND" , [](Node* node)
	{
		return buildExecutable<PlaySoundMessage>(node, &LetterBox::ADD_SENDER_PlaySoundMessage);
	} });

	builders.insert({ "MOVING_SOUND" , [](Node* node)
	{
		return buildExecutable<PlayMovingSoundMessage>(node, &LetterBox::ADD_SENDER_PlayMovingSoundMessage);
	} });

	builders.insert({ "TEXT_MESH" , [](Node* node) -> Executable
	{
		return buildExecutable<TextMeshMessage>(node, &LetterBox::ADD_SENDER_TextMeshMessage);
	} });
}

void SendMessageActionBuilder::initialiseDevConsoleBuilders()
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
			DeliverySystem::getPostman()->insertMessage(TextMessage(destination, text));
		};
	} });

	devConsoleBuilder.insert({ "relativetransform" , [](std::vector<std::string> line)
	{
		RelativeTransformMessage message = RelativeTransformMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "applyforce" , [](std::vector<std::string> line)
	{
		ApplyForceMessage message = ApplyForceMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "applyimpulse" , [](std::vector<std::string> line)
	{
		ApplyImpulseMessage message = ApplyImpulseMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "addscoreholder" , [](std::vector<std::string> line)
	{
		AddScoreHolderMessage message = AddScoreHolderMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "movegameobject" , [](std::vector<std::string> line)
	{
		MoveGameObjectMessage message = MoveGameObjectMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "scalegameobject" , [](std::vector<std::string> line)
	{
		ScaleGameObjectMessage message = ScaleGameObjectMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });

	devConsoleBuilder.insert({ "rotategameobject" , [](std::vector<std::string> line)
	{
		RotateGameObjectMessage message = RotateGameObjectMessage::tokensToMessage(line);

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	} });
}
