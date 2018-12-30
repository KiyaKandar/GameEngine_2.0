#include "PlayerInputMessage.h"

#include "../Input/Players/Player.h"

PlayerInputMessage::PlayerInputMessage(const std::string& destinationName, Player* player, ButtonInputData data)
	: Message(destinationName, PLAYER_INPUT)
{
	this->player = player;
	this->data = data;

	AddIntegerInformation("playerID", std::to_string(player->getPlayerID()));
	AddIntegerInformation("playerName", "player" + std::to_string(player->getPlayerID()));
	AddIntegerInformation("key", std::to_string(data.key));
}

PlayerInputMessage::~PlayerInputMessage()
{
}
