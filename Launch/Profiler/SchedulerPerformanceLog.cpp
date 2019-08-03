#include "SchedulerPerformanceLog.h"

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

SchedulerPerformanceLog* SchedulerPerformanceLog::schedulerPerformanceLog = nullptr;

const float MILLISECONDS_PER_SECOND = 1000.0f;

SchedulerPerformanceLog::SchedulerPerformanceLog()
{
	auto time = std::time(nullptr);
	std::ofstream logFile;
	logFile.open("SchedulerPerformanceLog.txt", std::ios_base::app);

	logFile << "----------------------------------------------------------------" << std::endl;
	logFile << "Session			-		" << std::put_time(std::localtime(&time), "%d-%m-%Y %H-%M-%S") << std::endl;
	logFile << "----------------------------------------------------------------" << std::endl;

	logFile.close();
}

void SchedulerPerformanceLog::AddElapsedFrameTime(const float elapsedFrameTime)
{
	totalFrameTime += elapsedFrameTime;
	++numberOfElapsedFrames;
}

void SchedulerPerformanceLog::LogAverageFrameTimeToFile()
{
	const float averageFrameTime = totalFrameTime / float(numberOfElapsedFrames);
	const float averageFrameRate = MILLISECONDS_PER_SECOND / averageFrameTime;

	std::ofstream logFile;
	logFile.open("SchedulerPerformanceLog.txt", std::ios_base::app);
	logFile << "Session Section Id: " << std::to_string(sectionId) << std::endl;
	logFile << "Average Frame Time (ms): " << std::to_string(averageFrameTime) << std::endl;
	logFile << "Average Frame Rate (fp/s): " << std::to_string(averageFrameRate) << std::endl;
	logFile << std::endl;

	logFile.close();
}

void SchedulerPerformanceLog::EndLoggedSection()
{
	totalFrameTime = 0.0f;
	numberOfElapsedFrames = 0;
	++sectionId;
}