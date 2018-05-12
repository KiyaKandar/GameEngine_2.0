#include "PlayerInputMessage.h"

#include "../Input/Players/Player.h"

PlayerInputMessage::PlayerInputMessage(const std::string& destinationName, Player* player, ButtonInputData data)
	: Message(destinationName, PLAYER_INPUT)
{
	this->player = player;
	this->data = data;

	addIntegerInformation("playerID", std::to_string(player->getPlayerID()));
	addIntegerInformation("playerName", "player" + std::to_string(player->getPlayerID()));
	addIntegerInformation("key", std::to_string(data.key));
}

PlayerInputMessage::~PlayerInputMessage()
{
}
