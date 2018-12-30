#pragma once

#include "Windows.h"
#include <vector>
#include <string>
#include <unordered_map>


class GameTimer
{
public:
	GameTimer();
	explicit GameTimer(std::string timerName);
	~GameTimer();

	float GetMillisecondsSinceStart();
	float GetTimeSinceLastRetrieval();

	void BeginTimedSection();
	void EndTimedSection();
	float GetTimeTakenForSection();

	void BeginChildTimedSection(std::string childTimerName);
	void EndChildTimedSection(std::string childTimerName);

	void AddChildTimer(std::string childTimerName);
	GameTimer* GetChildTimer(std::string timerName);

	std::vector<GameTimer*> GetAllChildTimers();

	std::string GetTimerName()
	{
		return timerName;
	}

private:
	LARGE_INTEGER start;
	LARGE_INTEGER ticksPerSecond;
	float timeSinceLastRetrieval;

	std::string timerName;
	std::unordered_map<std::string, GameTimer> childTimers;
	std::vector<GameTimer*> childTimerBuffer;

	float timeTakenForSection;
	float sectionStartTime;
	float sectionEndTime;
};