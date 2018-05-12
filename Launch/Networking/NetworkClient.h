#pragma once

#include "../Systems/Subsystem.h"
#include "../Input/Recorders/InputRecorder.h"
#include <Utilities/NetworkBase.h>
#include <enet\enet.h>
#include <set>
#include "DeadReckoning.h"
#include "../Utilities/Text/PeriodicTextModifier.h"
#include "MinionDeadReckoning.h"

class GameplaySystem;
class PlayerBase;
class Database;

struct NetworkedCollision
{
	int playerID;
	int colliderIndex;
	int offset;
};

struct RandomIntegers
{
	int r1;
	int others[10];
	bool first;
};

struct PlayerScore
{
	int playerID;
	int playerScore;
	int offset[2];
};

class NetworkClient : public Subsystem
{
public:
	NetworkClient(InputRecorder* keyboardAndMouse, Database* database,
		PlayerBase* playerbase, GameplaySystem* gameplay);
	~NetworkClient();

	void updateNextFrame(const float& deltaTime) override;

	void connectToServer();
	void waitForOtherClients(int numberToWaitFor);

private:
	void broadcastKinematicState();
	void broadcastMinionState();
	void broadcastPlayerScores();
	void broadcastCollision();

	void updateDeadReckoningForConnectedClients();
	void updateDeadReckoningForMinions();

	void displayPlayerScores();
	
	void processNetworkMessages(const float& deltaTime);

	int clientID;
	int numberOfOtherPlayersToWaitFor = 0;
	int numberOfOtherJoinedPlayers = 0;

	NetworkBase network;
	ENetPeer* serverConnection;

	InputRecorder* keyboardAndMouse;
	PlayerBase* playerbase; 
	GameplaySystem* gameplay;
	Database* database;
	std::vector<GameObject*> objectsToToTransmitStatesFor;

	bool connectedToServer;
	bool joinedGame;
	bool inLobby = false;

	NetworkedCollision networkedCollision;
	bool transmitNetworkedCollision = false;
	std::vector<std::string> colliders;

	std::map<GameObject*, DeadReckoning> clientDeadReckonings;
	std::map<GameObject*, MinionDeadReckoning> minionDeadReckonings;

	std::set<std::string> recievedClients;
	float timeSinceLastBroadcast = 0.0f;
	float msCounter = 0.0f;
	float updateRealTimeAccum = 0.0f;

	PeriodicTextModifier waitingInLobbyText;
	int numberOfFramesInLobby = 0;
	int numDots = 0;

	std::map<std::string, int> playerScores;

	std::vector<PlayerScore> scoresToSend;
};

