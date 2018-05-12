#pragma once

#include "../../Input/Players/Player.h"

#include <functional>
#include <unordered_map>

typedef std::function<void(Player*)> PlayerAction;

class InputActionMap
{
public:
	InputActionMap() = default;
	explicit InputActionMap(const int& playerID);
	~InputActionMap();

	void attachKeyToAction(const int& key, PlayerAction action);
	void executeAction(const int& key, Player* player);

	const int getAssociatedPlayerID() const
	{
		return playerID;
	}

private:
	std::unordered_map<int, PlayerAction> mapping;
	int playerID;
};

