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
	static bool isJoinGameMessage(ENetPacket* packet);
	static bool isKinematicStateMessage(ENetPacket* packet);

	static int getClientNumber(ENetPacket* joinGamePacket);
	static void joinGame(int clientID, PlayerBase* playerbase, GameplaySystem* game,
		InputRecorder* playerInputRecorder);
	static GameObject* getUpdatedDeadReckoningGameObject(std::string objectName,
		KinematicState& kinematicState, Database* database);
	static GameObject* getUpdatedDeadReckoningGameObject(std::string objectName,
		MinionKinematicState& kinematicState, Database* database);
};

