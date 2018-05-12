#include "GameTimer.h"

#include <ctime>

GameTimer::GameTimer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	timeSinceLastRetrieval = getMillisecondsSinceStart();
	timeTakenForSection = 0.0f;
	sectionStartTime = 0.0f;
	sectionEndTime = 0.0f;
}

GameTimer::GameTimer(std::string timerName)
{
	this->timerName = timerName;

	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	timeSinceLastRetrieval = getMillisecondsSinceStart();
	timeTakenForSection = 0.0f;
	sectionStartTime = 0.0f;
	sectionEndTime = 0.0f;
}

GameTimer::~GameTimer()
{
}

float GameTimer::getMillisecondsSinceStart()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	return (float)((t.QuadPart - start.QuadPart) * 1000.0 / ticksPerSecond.QuadPart);
}

float GameTimer::getTimeSinceLastRetrieval()
{
	float startTime = getMillisecondsSinceStart();
	float deltaTime = startTime - timeSinceLastRetrieval;

	timeSinceLastRetrieval = startTime;

	return deltaTime;
}

void GameTimer::beginTimedSection()
{
	sectionStartTime = getMillisecondsSinceStart();
}

void GameTimer::endTimedSection()
{
	sectionEndTime = getMillisecondsSinceStart();
	timeTakenForSection = sectionEndTime - sectionStartTime;
}

float GameTimer::getTimeTakenForSection()
{
	return timeTakenForSection;
}

void GameTimer::beginChildTimedSection(std::string childTimerName)
{
	childTimers.at(childTimerName).beginTimedSection();
}

void GameTimer::endChildTimedSection(std::string childTimerName)
{
	childTimers.at(childTimerName).endTimedSection();
}

void GameTimer::addChildTimer(std::string childTimerName)
{
	childTimers.insert({ childTimerName, GameTimer(childTimerName) });
	childTimerBuffer.push_back(&childTimers.at(childTimerName));
}

GameTimer* GameTimer::getChildTimer(std::string timerName)
{
	return &childTimers.at(timerName);
}

std::vector<GameTimer*> GameTimer::getAllChildTimers()
{
	return childTimerBuffer;
}