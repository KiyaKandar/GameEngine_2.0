#pragma once

#include "Subsystem.h"
#include "../Communication/MessageStorage.h"

#include <vector>
#include <memory>
#include "../Threading/ThreadPool/ThreadPool.h"
#include "Rendering/RenderingSystem.h"

class LetterBox;
class Profiler;
class GameTimer;

class System
{
public:
	System(ThreadPool* threadPool);
	~System();

	void UpdateNextSystemFrame();
	void StartConcurrentSubsystems();
	void SynchroniseAndStopConcurrentSubsystems();

	void AddSubsystem(Subsystem* subsystem);
	void AddConcurrentSubsystem(Subsystem* subsystem);

	void RemoveSubsystem(std::string subsystemName);

	void RegisterWithProfiler(Profiler* profiler) const;

	std::vector<Subsystem*> GetSubSystems();

private:
	MessageStorage messageBuffers;
	LetterBox* letterBox;
	ThreadPool* threadPool;
	GameTimer* timer;

	std::vector<Subsystem*> subsystems;
	std::vector<Subsystem*> concurrentSubsystems;
	vector<TaskFuture<void>> updates;
	bool running;
};
