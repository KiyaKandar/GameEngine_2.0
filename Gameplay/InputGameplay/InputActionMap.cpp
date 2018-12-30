#include "InputActionMap.h"

InputActionMap::InputActionMap(const int& playerID)
{
	this->playerID = playerID;
}

InputActionMap::~InputActionMap()
{
}

void InputActionMap::AttachKeyToAction(const int& key, PlayerAction action)
{
	mapping.insert({ key, action });
}

void InputActionMap::ExecuteAction(const int& key, Player* player)
{
	mapping.at(key)(player);
}
