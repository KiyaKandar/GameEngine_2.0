#pragma once

#include "../Systems/Subsystem.h"
#include "../Input/Devices/Keyboard.h"
#include "MemoryWatcher.h"

#include <string>
#include <vector>
#include "VisualProfiler.h"

struct Worker;
class GameTimer;
class Database;
class FPSCounter;

class Profiler : public Subsystem
{
public:
	Profiler(Keyboard* keyboard, Database* database, FPSCounter* fpsCounter);

	~Profiler()
	{
	}

	void UpdateNextFrame(const float& deltatime) override;
	void AddSubsystemTimer(std::string name, GameTimer* timer);
	void RegisterWorkers(std::vector<Worker>* workers, Worker* mainThreadWorker);

private:
	void DisplayWorkerDebugInfo();
	void UpdateProfiling();

	void UpdateFps();
	void UpdateMemory();
	void UpdateTimers();

	void DisplayChildTimers();
	void SaveProfilingInfo(GameTimer* parentTimer, int currentDepth, float parentXOffset);

	int numTimers;
	int numAdded = 0;
	int numFramesUpdated = 0;

	Database* database;
	Keyboard* keyboard;

	MemoryWatcher memoryWatcher;
	FPSCounter* fpsCounter;
	VisualProfiler visualProfiler;
	int visualProfilerMode = 0;

	map<string, GameTimer*> timers;
	std::vector<TextMeshMessage> messages;
	std::vector<std::string> externalText;

	std::vector<Worker>* workers;
	Worker* mainThreadWorker;

	int depth = -1;
	bool profilerEnabled = false;
	bool workerDebugEnabled = false;

	NCLVector4 defaultColour = NCLVector4(1, 1, 1, 1);
	NCLVector2 defaultSize = NCLVector2(16, 16);
	float nextLine = 0.0f;
	bool timersDisplayed = true;

	SinglePressKeyListener f4Listener;
	SinglePressKeyListener f5Listener;
	SinglePressKeyListener f6Listener;
	SinglePressKeyListener f10Listener;
};
