
/******************************************************************************
Class: Net1_Client
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

:README:
- In order to run this demo, we also need to run "Tuts_Network_Client" at the same time.
- To do this:-
	1. right click on the entire solution (top of the solution exporerer) and go to properties
	2. Go to Common Properties -> Statup Project
	3. Select Multiple Statup Projects
	4. Select 'Start with Debugging' for both "Tuts_Network_Client" and "Tuts_Network_Server"

- Now when you run the program it will build and run both projects at the same time. =]
- You can also optionally spawn more instances by right clicking on the specific project
and going to Debug->Start New Instance.




FOR MORE NETWORKING INFORMATION SEE "Tuts_Network_Client -> Net1_Client.h"



		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma comment(lib, "Utilities.lib")

#include <enet\enet.h>
#include <enet\include/enet/enet.h>
#include "../Utilities/GameTimer.h"
#include <Utilities/NetworkBase.h>
#include "../Utilities/Maths/Matrix4.h"

//Needed to get computer adapter IPv4 addresses via windows
#include <iphlpapi.h>
#include <random>
#pragma comment(lib, "IPHLPAPI.lib")

#define SERVER_PORT 1234
#define UPDATE_TIMESTEP (1.0f / 30.0f) //send 30 position updates per second

NetworkBase server;
GameTimer timer;
float accum_time = 0.0f;
float rotation = 0.0f;

void Win32_PrintAllAdapterIPAddresses();

struct KinematicState
{
	int clientID;

	NCLVector3 position;
	NCLVector3 linearVelocity;
	NCLVector3 linearAcceleration;
};

struct MinionKinematicState
{
	int minionIndex;
	int padding;

	NCLVector3 position;
	NCLVector3 linearVelocity;
	NCLVector3 linearAcceleration;
};

struct PlayerScore
{
	int playerID;
	int playerScore;
	int offset[2];
};

enum
{
	NEW_ID,
	CURRENT_NUMBER_OF_PLAYERS
};

struct IntegerData
{
	int type;
	int data;
};

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

int onExit(int exitcode)
{
	server.Release();
	system("pause");
	exit(exitcode);
}

int main(int arcg, char** argv)
{
	std::random_device rd;
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}

	//Initialize Server on Port 1234, with a possible 32 clients connected at any time
	if (!server.Initialize(SERVER_PORT, 32))
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		onExit(EXIT_FAILURE);
	}

	printf("Server Initiated\n");


	Win32_PrintAllAdapterIPAddresses();
	int numberOfPlayersConnected = 0;

	timer.GetMillisecondsSinceStart();
	while (true)
	{
		float dt = timer.GetMillisecondsSinceStart() * 0.001f;

		//Handle All Incoming Packets and Send any enqued packets
		server.ServiceNetwork(dt, [&](const ENetEvent& evnt)
		{
			switch (evnt.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("- New Client Connected\n");

				IntegerData message;
				message.type = NEW_ID;
				message.data = numberOfPlayersConnected;

				ENetPacket* idPacket = enet_packet_create(&message,
					sizeof(IntegerData), 0);
				enet_peer_send(evnt.peer, 0, idPacket);
				++numberOfPlayersConnected;

				IntegerData numberOfPlayersMessage;
				numberOfPlayersMessage.type = CURRENT_NUMBER_OF_PLAYERS;
				numberOfPlayersMessage.data = numberOfPlayersConnected;

				ENetPacket* packet = enet_packet_create(&numberOfPlayersMessage, sizeof(IntegerData), 0);
				enet_host_broadcast(server.m_pNetwork, 0, packet);

				std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
				std::uniform_int_distribution<int> uni(0, 2); // guaranteed unbiased
				auto r1 = uni(rng);

				std::mt19937 rng2(rd());    // random-number engine used (Mersenne-Twister in this case)
				std::uniform_int_distribution<int> uni2(-6, 6); // guaranteed unbiased

				RandomIntegers rndInts;
				rndInts.r1 = r1;

				for (int i = 0; i < 10; ++i)
				{
					rndInts.others[i] = uni2(rng2);
				}

				rndInts.first = true;

				ENetPacket* packet2 = enet_packet_create(&rndInts, sizeof(RandomIntegers), 0);
				enet_host_broadcast(server.m_pNetwork, 0, packet2);
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				if (evnt.packet->dataLength == sizeof(KinematicState))
				{
					KinematicState recievedPlayerPacket;
					memcpy(&recievedPlayerPacket, evnt.packet->data, sizeof(KinematicState));

					ENetPacket* packet = enet_packet_create(&recievedPlayerPacket, sizeof(KinematicState), 0);
					enet_host_broadcast(server.m_pNetwork, 0, packet);
				}
				else if (evnt.packet->dataLength == sizeof(MinionKinematicState))
				{
					MinionKinematicState recievedPlayerPacket;
					memcpy(&recievedPlayerPacket, evnt.packet->data, sizeof(MinionKinematicState));

					ENetPacket* packet = enet_packet_create(&recievedPlayerPacket, sizeof(MinionKinematicState), 0);
					enet_host_broadcast(server.m_pNetwork, 0, packet);
				}
				
				else if (evnt.packet->dataLength == sizeof(NetworkedCollision))
				{
					NetworkedCollision recievedPlayerPacket;
					memcpy(&recievedPlayerPacket, evnt.packet->data, sizeof(NetworkedCollision));

					ENetPacket* packet = enet_packet_create(&recievedPlayerPacket, sizeof(NetworkedCollision), 0);
					enet_host_broadcast(server.m_pNetwork, 0, packet);


					std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
					std::uniform_int_distribution<int> uni(0, 2); // guaranteed unbiased
					auto r1 = uni(rng);

					std::mt19937 rng2(rd());    // random-number engine used (Mersenne-Twister in this case)
					std::uniform_int_distribution<int> uni2(-6, 6); // guaranteed unbiased
					auto r2 = uni2(rng2);
					auto r3 = uni2(rng2);

					RandomIntegers rndInts;
					rndInts.r1 = r1;

					for (int i = 0; i < 10; ++i)
					{
						rndInts.others[i] = uni2(rng2);
					}

					rndInts.first = false;

					ENetPacket* packet2 = enet_packet_create(&rndInts, sizeof(RandomIntegers), 0);
					enet_host_broadcast(server.m_pNetwork, 0, packet2);

				}
				else if (evnt.packet->dataLength == sizeof(PlayerScore))
				{
					PlayerScore recievedPlayerPacket;
					memcpy(&recievedPlayerPacket, evnt.packet->data, sizeof(PlayerScore));

					ENetPacket* packet = enet_packet_create(&recievedPlayerPacket, sizeof(PlayerScore), 0);
					enet_host_broadcast(server.m_pNetwork, 0, packet);
				}

				enet_packet_destroy(evnt.packet);
			}
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				{
				printf("- Client %d has disconnected.\n", evnt.peer->incomingPeerID);
				--numberOfPlayersConnected;
				}
				break;

			}
		});

		Sleep(0);
	}

	system("pause");
	server.Release();
}




//Yay Win32 code >.>
//  - Grabs a list of all network adapters on the computer and prints out all IPv4 addresses associated with them.
void Win32_PrintAllAdapterIPAddresses()
{
	//Initially allocate 5KB of memory to store all adapter info
	ULONG outBufLen = 5000;
	

	IP_ADAPTER_INFO* pAdapters = NULL;
	DWORD status = ERROR_BUFFER_OVERFLOW;

	//Keep attempting to fit all adapter info inside our buffer, allocating more memory if needed
	// Note: Will exit after 5 failed attempts, or not enough memory. Lets pray it never comes to this!
	for (int i = 0; i < 5 && (status == ERROR_BUFFER_OVERFLOW); i++)
	{
		pAdapters = (IP_ADAPTER_INFO *)malloc(outBufLen);
		if (pAdapters != NULL) {

			//Get Network Adapter Info
			status = GetAdaptersInfo(pAdapters, &outBufLen);

			// Increase memory pool if needed
			if (status == ERROR_BUFFER_OVERFLOW) {
				free(pAdapters);
				pAdapters = NULL;
			}
			else {
				break;
			}
		}
	}

	
	if (pAdapters != NULL)
	{
		//Iterate through all Network Adapters, and print all IPv4 addresses associated with them to the console
		// - Adapters here are stored as a linked list termenated with a NULL next-pointer
		IP_ADAPTER_INFO* cAdapter = &pAdapters[0];
		while (cAdapter != NULL)
		{
			IP_ADDR_STRING* cIpAddress = &cAdapter->IpAddressList;
			while (cIpAddress != NULL)
			{
				printf("\t - Listening for connections on %s:%u\n", cIpAddress->IpAddress.String, SERVER_PORT);
				cIpAddress = cIpAddress->Next;
			}
			cAdapter = cAdapter->Next;
		}

		free(pAdapters);
	}
	
}