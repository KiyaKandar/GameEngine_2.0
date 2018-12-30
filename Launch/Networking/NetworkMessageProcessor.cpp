#include "NetworkMessageProcessor.h"

#include "../../Resource Management/Database/Database.h"
#include "../Gameplay/GameObject.h"
#include "../Physics/PhysicsNode.h"

bool NetworkMessageProcessor::IsJoinGameMessage(ENetPacket* packet)
{
	return packet->dataLength == sizeof(int);
}

bool NetworkMessageProcessor::IsKinematicStateMessage(ENetPacket* packet)
{
	return packet->dataLength == sizeof(KinematicState);
}

int NetworkMessageProcessor::GetClientNumber(ENetPacket* joinGamePacket)
{
	int clientID;
	memcpy(&clientID, joinGamePacket->data, sizeof(int));

	return clientID;
}

void NetworkMessageProcessor::JoinGame(int clientID, PlayerBase* playerbase, GameplaySystem* game,
	InputRecorder* playerInputRecorder)
{
	playerbase->AddNewPlayer(playerInputRecorder, clientID);
	game->ConnectPlayerbase(playerbase);
}

GameObject* NetworkMessageProcessor::GetUpdatedDeadReckoningGameObject(std::string objectName,
	KinematicState& kinematicState,
	Database* database)
{
	GameObject* client = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(objectName));

	client->GetPhysicsNode()->constantAcceleration = true;
	client->GetPhysicsNode()->SetLinearVelocity(kinematicState.linearVelocity);
	client->GetPhysicsNode()->SetAcceleration(kinematicState.linearAcceleration);

	return client;
}

GameObject* NetworkMessageProcessor::GetUpdatedDeadReckoningGameObject(std::string objectName,
	MinionKinematicState& kinematicState,
	Database* database)
{
	GameObject* client = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(objectName));

	client->GetPhysicsNode()->constantAcceleration = true;
	client->GetPhysicsNode()->SetLinearVelocity(kinematicState.linearVelocity);
	client->GetPhysicsNode()->SetAcceleration(kinematicState.linearAcceleration);

	return client;
}
