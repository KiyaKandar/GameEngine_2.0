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

	float getMillisecondsSinceStart();
	float getTimeSinceLastRetrieval();

	void beginTimedSection();
	void endTimedSection();
	float getTimeTakenForSection();

	void beginChildTimedSection(std::string childTimerName);
	void endChildTimedSection(std::string childTimerName);

	void addChildTimer(std::string childTimerName);
	GameTimer* getChildTimer(std::string timerName);

	std::vector<GameTimer*> getAllChildTimers();

	std::string getTimerName()
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