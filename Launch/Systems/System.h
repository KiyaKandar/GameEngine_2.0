#pragma once

#include "Subsystem.h"
#include "../Communication/MessageStorage.h"

#include <vector>
#include <memory>
#include "Rendering/RenderingSystem.h"
#include "Launch/Threading/Scheduler/ProcessScheduler.h"

class LetterBox;
class Profiler;
class GameTimer;

class System
{
public:
	System();
	~System();

	void StartConcurrentSubsystems();
	void SynchroniseAndStopConcurrentSubsystems();

	void AddSubsystem(Subsystem* subsystem);
	void AddConcurrentSubsystem(Subsystem* subsystem);

	void RemoveSubsystem(std::string subsystemName);

	void RegisterWithProfiler(Profiler* profiler) const;

	std::vector<Subsystem*> GetSubSystems();

	static std::atomic_bool stop;

	void UpdateNextSystemFrame();

private:
	GameTimer* timer;

	std::vector<Subsystem*> subsystems;
	std::vector<Subsystem*> concurrentSubsystems;
};
