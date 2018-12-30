#include "GameTimer.h"

#include <ctime>

GameTimer::GameTimer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	timeSinceLastRetrieval = GetMillisecondsSinceStart();
	timeTakenForSection = 0.0f;
	sectionStartTime = 0.0f;
	sectionEndTime = 0.0f;
}

GameTimer::GameTimer(std::string timerName)
{
	this->timerName = timerName;

	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	timeSinceLastRetrieval = GetMillisecondsSinceStart();
	timeTakenForSection = 0.0f;
	sectionStartTime = 0.0f;
	sectionEndTime = 0.0f;
}

GameTimer::~GameTimer()
{
}

float GameTimer::GetMillisecondsSinceStart()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	return (float)((t.QuadPart - start.QuadPart) * 1000.0 / ticksPerSecond.QuadPart);
}

float GameTimer::GetTimeSinceLastRetrieval()
{
	float startTime = GetMillisecondsSinceStart();
	float deltaTime = startTime - timeSinceLastRetrieval;

	timeSinceLastRetrieval = startTime;

	return deltaTime;
}

void GameTimer::BeginTimedSection()
{
	sectionStartTime = GetMillisecondsSinceStart();
}

void GameTimer::EndTimedSection()
{
	sectionEndTime = GetMillisecondsSinceStart();
	timeTakenForSection = sectionEndTime - sectionStartTime;
}

float GameTimer::GetTimeTakenForSection()
{
	return timeTakenForSection;
}

void GameTimer::BeginChildTimedSection(std::string childTimerName)
{
	childTimers.at(childTimerName).BeginTimedSection();
}

void GameTimer::EndChildTimedSection(std::string childTimerName)
{
	childTimers.at(childTimerName).EndTimedSection();
}

void GameTimer::AddChildTimer(std::string childTimerName)
{
	childTimers.insert({ childTimerName, GameTimer(childTimerName) });
	childTimerBuffer.push_back(&childTimers.at(childTimerName));
}

GameTimer* GameTimer::GetChildTimer(std::string timerName)
{
	return &childTimers.at(timerName);
}

std::vector<GameTimer*> GameTimer::GetAllChildTimers()
{
	return childTimerBuffer;
}