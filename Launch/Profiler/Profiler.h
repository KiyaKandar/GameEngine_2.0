#pragma once

#include "../Systems/Subsystem.h"
#include "../Input/Devices/Keyboard.h"
#include "../../Communication/MessageSenders/TrackedGroupMessageSender.h"
#include "../../Communication/MessageSenders/TrackedMessageSender.h"
#include "MemoryWatcher.h"

#include <string>
#include <vector>

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

private:
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
	map<string, GameTimer*> timers;
	std::vector<TextMeshMessage> messages;
	std::vector<std::string> externalText;

	int depth = -1;
	bool profilerEnabled = false;

	NCLVector4 defaultColour = NCLVector4(1, 1, 1, 1);
	NCLVector2 defaultSize = NCLVector2(16, 16);
	float nextLine = 0.0f;

	TrackedGroupMessageSender<TextMeshMessage> profilerTextSender;
	bool timersDisplayed = true;

	SinglePressKeyListener f5Listener;
	SinglePressKeyListener f6Listener;
};
