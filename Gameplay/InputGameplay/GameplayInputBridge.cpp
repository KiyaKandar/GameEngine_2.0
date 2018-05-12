#include "GameplayInputBridge.h"

void GameplayInputBridge::addInputActionMapForPlayer(const InputActionMap& mapping)
{
	actionsForEachPlayer.insert({mapping.getAssociatedPlayerID(), mapping});
}

void GameplayInputBridge::processPlayerInputMessage(const PlayerInputMessage& message)
{
	actionsForEachPlayer.at(message.player->getPlayerID()).executeAction(message.data.key, message.player);
}
