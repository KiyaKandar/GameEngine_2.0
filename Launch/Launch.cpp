#pragma comment(lib, "Interface.lib")
#pragma comment(lib, "Utilities.lib")
#pragma comment(lib, "Input.lib")
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "Communication.lib")
#pragma comment(lib, "Gameplay.lib")
#pragma comment(lib, "Resource Management.lib")
#pragma comment(lib, "Audio.lib")
#pragma comment(lib, "Physics.lib")

#include <enet\enet.h>
#include "Systems\System.h"
#include "Launch/Startup.h"
#include "Communication/SendMessageActionBuilder.h"

int main()
{
	if (enet_initialize() != 0)
	{
		return -1;
	}

	SendMessageActionBuilder::InitialiseBuilders();
	ThreadPool threadPool;

	Startup startup(&threadPool);
	startup.InitialiseRenderingSystem();

	bool loadedSubsystems = false;

	threadPool.SubmitJob([&startup = startup, &loadedSubsystems = loadedSubsystems]()
	{
		startup.InitialiseSubsystems();
		startup.LoadMainMenu();

		loadedSubsystems = true;
	});

	while (!loadedSubsystems)
	{
		startup.RenderLoadingScreen();
	}

	startup.StartRenderingSystem();
	startup.SetupMeshes();
	startup.StartUserInterface();

	startup.StartGameLoop();

	return 0;
}
