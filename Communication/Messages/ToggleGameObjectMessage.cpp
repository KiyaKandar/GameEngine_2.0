#include "ToggleGameObjectMessage.h"

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

ToggleGameObjectMessage::ToggleGameObjectMessage(const std::string& destinationName, std::string gameObjectID,
	bool isEnabled)
	: Message(destinationName, TOGGLE_GAMEOBJECT)
{
	this->gameObjectID = gameObjectID;
	this->isEnabled = isEnabled;
}


ToggleGameObjectMessage::~ToggleGameObjectMessage()
{
}

ToggleGameObjectMessage ToggleGameObjectMessage::builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	bool isEnabled;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			destination = childNode->value;
		}
		else if (childNode->nodeType == "resource")
		{
			object = childNode->value;
		}
		else if (childNode->nodeType == "enabled")
		{
			isEnabled = stob(childNode->value);
		}
	}
	return ToggleGameObjectMessage(destination, object, isEnabled);
}

bool ToggleGameObjectMessage::stob(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}