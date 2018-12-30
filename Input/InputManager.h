#pragma once

#include <vector>
#include "../Launch/Systems/Subsystem.h"
#include "Players/PlayerBase.h"
#include "Players/Player.h"
#include "InputControl.h"
#include "../Input/Devices/Window.h"
#include "../Communication/MessageSenders/TrackedGroupMessageSender.h"


class InputManager : public Subsystem
{
public:
	InputManager(PlayerBase* playerbase);
	~InputManager();

	void UpdateNextFrame(const float& deltatime) override;

	PlayerBase* GetPlayerbase() const
	{
		return playerbase;
	}

private:
	void SendInputMessagesForKeys(std::vector<ButtonInputData>& inputData, Player* player);

	PlayerBase* playerbase;
	Window* window;
	InputControl inputControl;

	std::vector<std::pair<std::string, int>> keysToBlock;
	std::vector<std::pair<std::string, int>> keysToUnblock;
	std::vector<std::pair<std::string, int>> blockedKeysForEachPlayer;

	bool blocked;
	TrackedGroupMessageSender<PlayerInputMessage> inputMessageSender;
};
