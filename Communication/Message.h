#pragma once

#include <string>
#include <unordered_map>

enum MessageType
{
	PLAYER_INPUT,
	DUMMY_TYPE,
	RELATIVE_TRANSFORM,
	TEXT,
	PLAY_SOUND,
	STOP_SOUND,
	MOVING_SOUND,
	TOGGLE_GRAPHICS_MODULE,
	APPLY_FORCE,
	MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT,
	APPLY_IMPULSE,
	COLLISION,
	PREPARE_PAINT_SURFACE,
	PAINT_TRAIL_FOR_GAMEOBJECT,
	UPDATE_POSITION,
	TEXT_MESH_MESSAGE,
	ADD_SCORE_HOLDER,
	ABSOLUTE_TRANSFORM,
	MOVE_GAMEOBJECT,
	SCALE_GAMEOBJECT,
	ROTATE_GAMEOBJECT,
	TOGGLE_GAMEOBJECT,
	PLAY_ANIMATION,
	TOGGLE_PLAYER_INPUT,
	DEBUG_LINE,
	DEBUG_SPHERE,
	UIQUAD
};

class Message
{
public:
	Message() {}
	Message(const std::string& destinationName, MessageType type);
	
	virtual ~Message() {}

	const std::string GetDestination() const
	{ 
		return destination;
	}

	const MessageType GetMessageType() const
	{ 
		return type; 
	}

	virtual std::string GetDataField(std::string name)
	{
		if (name == "destination")
		{
			return destination;
		}
		else if (name == "type")
		{
			return messageTypeData.at(type);
		}
		else
		{
			return integerInformation.at(name);
		}
	}
	
	void AddIntegerInformation(std::string key, std::string value)
	{
		integerInformation.insert({ key, value });
	}

	bool processed = false;
	bool* senderAvailable = nullptr;

protected:
	std::string destination;
	MessageType type;

	std::unordered_map<int, std::string> messageTypeData;
	std::unordered_map<std::string, std::string> integerInformation;
};

