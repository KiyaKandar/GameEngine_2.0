#include "Message.h"

Message::Message(const std::string& destinationName,  MessageType type)
{
	this->destination = destinationName;
	this->type = type;
	
	messageTypeData.insert({ PLAYER_INPUT, "PLAYER_INPUT" });
	messageTypeData.insert({ DUMMY_TYPE, "DUMMY_TYPE" });
	messageTypeData.insert({ RELATIVE_TRANSFORM, "TRANSLATE_SCENE_NODE" });
	messageTypeData.insert({ TEXT, "TEXT" });
}

