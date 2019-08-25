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
#include "Threading/Scheduler/SubsystemScheduler.h"
#include "Communication/LetterBox.h"
#include "Profiler/SchedulerPerformanceLog.h"

int main()
{
	if (enet_initialize() != 0)
	{
		return -1;
	}

	SchedulerPerformanceLog::Create();
	ProcessScheduler::Create(new SubsystemScheduler());
	DeliverySystem::Provide(new LetterBox());
	SendMessageActionBuilder::InitialiseBuilders();

	Startup startup;
	bool loadedSubsystems = false;
	startup.InitialiseRenderingSystem();

	//TODO Need to put this on a generic scheduler, not a subsystem scheduler that wraps work in loops
	//ProcessScheduler::Retrieve()->RegisterProcess([&startup = startup, &loadedSubsystems = loadedSubsystems]()
	//{
		if (!loadedSubsystems)
		{
			startup.InitialiseSubsystems();
			startup.LoadMainMenu();
		}

		loadedSubsystems = true;
	//}, "loading");

	//ProcessScheduler::Retrieve()->AttachMainThreadProcess([&loadedSubsystems, &startup]()
	//{
	//	while (!loadedSubsystems)
	//	{
	//		startup.RenderLoadingScreen();
	//	}
	//}, "Loading Screen");

	//ProcessScheduler::Retrieve()->BeginWorkerProcesses();
	//ProcessScheduler::Retrieve()->CompleteWorkerProcesses();

	startup.StartRenderingSystem();
	startup.InitialiseGraphicalAssets();
	startup.StartUserInterface();

	startup.StartGameLoop();
	SchedulerPerformanceLog::Retrieve()->LogAverageFrameTimeToFile();
	SchedulerPerformanceLog::Destroy();

	return 0;
}
