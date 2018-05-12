#include "InputActionMap.h"

InputActionMap::InputActionMap(const int& playerID)
{
	this->playerID = playerID;
}

InputActionMap::~InputActionMap()
{
}

void InputActionMap::attachKeyToAction(const int& key, PlayerAction action)
{
	mapping.insert({ key, action });
}

void InputActionMap::executeAction(const int& key, Player* player)
{
	mapping.at(key)(player);
}
