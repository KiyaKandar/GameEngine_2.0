#pragma once

#include "NetworkBase.h"
#include "DeadReckoning.h"
#include "MinionDeadReckoning.h"
#include "GameplaySystem.h"
#include "Players/PlayerBase.h"

class Database;
class GameObject;

class NetworkMessageProcessor
{
public:
	static bool IsJoinGameMessage(ENetPacket* packet);
	static bool IsKinematicStateMessage(ENetPacket* packet);

	static int GetClientNumber(ENetPacket* joinGamePacket);
	static void JoinGame(int clientID, PlayerBase* playerbase, GameplaySystem* game,
		InputRecorder* playerInputRecorder);
	static GameObject* GetUpdatedDeadReckoningGameObject(std::string objectName,
		KinematicState& kinematicState, Database* database);
	static GameObject* GetUpdatedDeadReckoningGameObject(std::string objectName,
		MinionKinematicState& kinematicState, Database* database);
};
