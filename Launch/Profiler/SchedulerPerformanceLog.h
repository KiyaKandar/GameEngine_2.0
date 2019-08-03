#pragma once
class SchedulerPerformanceLog
{
public:
	static SchedulerPerformanceLog* Retrieve()
	{
		return schedulerPerformanceLog;
	}

	static void Create()
	{
		schedulerPerformanceLog = new SchedulerPerformanceLog();
	}

	static void Destroy()
	{
		delete schedulerPerformanceLog;
	}

	void AddElapsedFrameTime(const float elapsedFrameTime);
	void LogAverageFrameTimeToFile();
	void EndLoggedSection();

private:
	SchedulerPerformanceLog();

	float totalFrameTime = 0.0f;
	unsigned int numberOfElapsedFrames = 0;
	unsigned int sectionId = 0;

	static SchedulerPerformanceLog* schedulerPerformanceLog;
};

