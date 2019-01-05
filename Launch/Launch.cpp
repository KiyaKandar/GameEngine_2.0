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
#include "Threading/Scheduler/PersistentProcessScheduler.h"

int main()
{
	if (enet_initialize() != 0)
	{
		return -1;
	}

	SendMessageActionBuilder::InitialiseBuilders();
	ProcessScheduler::Create(new PersistentProcessScheduler());

	Startup startup;
	bool loadedSubsystems = false;
	startup.InitialiseRenderingSystem();

	ProcessScheduler::Retrieve()->RegisterProcess([&startup = startup, &loadedSubsystems = loadedSubsystems]()
	{
		startup.InitialiseSubsystems();
		startup.LoadMainMenu(); 

		loadedSubsystems = true;
	});

	ProcessScheduler::Retrieve()->BeginWorkerProcesses();

	while (!loadedSubsystems)
	{
		startup.RenderLoadingScreen();
	}

	startup.StartRenderingSystem();
	startup.InitialiseGraphicalAssets();
	startup.StartUserInterface();

	startup.StartGameLoop();

	return 0;
}
