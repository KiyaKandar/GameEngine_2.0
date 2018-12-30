#include "GameplayInputBridge.h"

void GameplayInputBridge::AddInputActionMapForPlayer(const InputActionMap& mapping)
{
	actionsForEachPlayer.insert({mapping.GetAssociatedPlayerId(), mapping});
}

void GameplayInputBridge::ProcessPlayerInputMessage(const PlayerInputMessage& message)
{
	actionsForEachPlayer.at(message.player->getPlayerID()).ExecuteAction(message.data.key, message.player);
}
