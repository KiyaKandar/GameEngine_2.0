#pragma once

#include "../../Communication/Messages/PlayerInputMessage.h"
#include "InputActionMap.h"

class GameplayInputBridge
{
public:
	GameplayInputBridge() {}
	~GameplayInputBridge() {}

	void AddInputActionMapForPlayer(const InputActionMap& mapping);
	void ProcessPlayerInputMessage(const PlayerInputMessage& message);

	void ClearInputActions()
	{
		actionsForEachPlayer.clear();
	}

private:
	std::unordered_map<int, InputActionMap> actionsForEachPlayer;
};

