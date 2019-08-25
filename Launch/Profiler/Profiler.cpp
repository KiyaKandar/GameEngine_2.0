#include "Profiler.h"

#include "../Resource Management/Database/Database.h"
#include "../Utilities/GameTimer.h"
#include "../Threading/Scheduler/SubsystemWorkload.h"
#include "FPSCounter.h"

const float Y_OFFSET = 320.0f;
const float NEXT_LINE_OFFSET = -12.9f;
const NCLVector3 TEXT_SIZE = NCLVector3(12.9f, 12.9f, 12.9f);
const NCLVector3 TEXT_COLOUR = NCLVector3(0, 1, 0);

Profiler::Profiler(Keyboard* keyboard, Database* database, FPSCounter* fpsCounter) : Subsystem("Profiler")
{
	this->fpsCounter = fpsCounter;
	this->database = database;
	this->keyboard = keyboard;

	incomingMessages = MessageProcessor::MessageProcessor(std::vector<MessageType>{MessageType::TEXT},
		DeliverySystem::GetPostman()->GetDeliveryPoint("Profiler"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [&externalText = externalText](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		externalText.push_back(textMessage->text);
	});

	memoryWatcher = MemoryWatcher(database->MaxSize(), database);

	f4Listener = SinglePressKeyListener(KEYBOARD_F4, keyboard);
	f5Listener = SinglePressKeyListener(KEYBOARD_F5, keyboard);
	f6Listener = SinglePressKeyListener(KEYBOARD_F6, keyboard);
	f10Listener = SinglePressKeyListener(KEYBOARD_F10, keyboard);
}

void Profiler::UpdateNextFrame(const float& deltatime)
{
	fpsCounter->CalculateFps(deltatime);

	if (f4Listener.KeyPressed())
	{
		workerDebugEnabled = !workerDebugEnabled;
	}

	if (f5Listener.KeyPressed())
	{
		profilerEnabled = !profilerEnabled;
		depth = -1;
	}

	if (f6Listener.KeyPressed())
	{
		++depth;
	}

	if (f10Listener.KeyPressed())
	{
		++visualProfilerMode;

		if (visualProfilerMode == 1)
		{
			visualProfiler.ToggleEnabled();
		}
		else if (visualProfilerMode == 2)
		{
			visualProfiler.TogglePrecisionMode();
		}
		if (visualProfilerMode == 3)
		{
			visualProfiler.ToggleEnabled();
			visualProfiler.TogglePrecisionMode();

			visualProfilerMode = 0;
		}
	}

	visualProfiler.DisplayVisualTimers();

	if (profilerEnabled)
	{
		++numFramesUpdated;

		if (numFramesUpdated >= 30)
		{
			numFramesUpdated = 0;
			messages.clear();

			memoryWatcher.Update();
			UpdateProfiling();
		}

		DisplayChildTimers();
	}
}

void Profiler::AddSubsystemTimer(string name, GameTimer* timer)
{
	timers.insert({name, timer});
	visualProfiler.AddTimerBar(name, timer);
}

void Profiler::RegisterWorkers(std::vector<Worker>* workers, Worker* mainThreadWorker)
{
	this->workers = workers;
	this->mainThreadWorker = mainThreadWorker;
}

void Profiler::DisplayWorkerDebugInfo()
{
	std::string text = "thread " + std::to_string(0) + "  " +  std::to_string(mainThreadWorker->currentWorkloadSize);
	messages.push_back(TextMeshMessage("RenderingSystem", text,
		NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
	nextLine += NEXT_LINE_OFFSET;

	const std::vector<SubsystemWorkload*>& workload = mainThreadWorker->assignedWorkload;
	for (int i = 0; i < workload.size(); ++i)
	{
		std::string workText = workload[i]->debugName + "  " + std::to_string(workload[i]->workloadSize);
		messages.push_back(TextMeshMessage("RenderingSystem", workText,
			NCLVector3(-480.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
		nextLine += NEXT_LINE_OFFSET;
	}

	for (int i = 0; i < workers->size(); ++i)
	{
		std::string text = "thread " + std::to_string(i + 1) + "  " + std::to_string((*workers)[i].currentWorkloadSize);
		messages.push_back(TextMeshMessage("RenderingSystem", text,
			NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
		nextLine += NEXT_LINE_OFFSET;

		const std::vector<SubsystemWorkload*>& workload1 = (*workers)[i].assignedWorkload;
		for (int i = 0; i < workload1.size(); ++i)
		{
			std::string workText1 = workload1[i]->debugName + "  " + std::to_string(workload1[i]->workloadSize);
			messages.push_back(TextMeshMessage("RenderingSystem", workText1,
				NCLVector3(-480.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
			nextLine += NEXT_LINE_OFFSET;
		}
	}
}

void Profiler::UpdateProfiling()
{
	nextLine = Y_OFFSET;

	if (timersDisplayed)
	{
		UpdateFps();

		if (workerDebugEnabled)
		{
			DisplayWorkerDebugInfo();
		}

		if (depth >= 0)
		{
			UpdateMemory();
			UpdateTimers();

			for (std::string text : externalText)
			{
				messages.push_back(TextMeshMessage("RenderingSystem", text,
					NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
				nextLine += NEXT_LINE_OFFSET;
			}

			timersDisplayed = false;
		}
	}
}

void Profiler::UpdateFps()
{
	TextMeshMessage message("RenderingSystem", "FPS : " + std::to_string(fpsCounter->fps),
		NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true);

	messages.push_back(message);
	nextLine += NEXT_LINE_OFFSET;
}

void Profiler::UpdateMemory()
{
	messages.push_back(TextMeshMessage("RenderingSystem", "Memory Usage : " + std::to_string(memoryWatcher.percent),
		NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
	nextLine += NEXT_LINE_OFFSET;

	for (size_t i = 0; i < database->GetAllTables().size(); i++)
	{
		std::string text = database->GetAllTables()[i]->GetName() + ": " +
			std::to_string(database->GetAllTables()[i]->GetAllResources()->GetCurrentSize());
		messages.push_back(TextMeshMessage("RenderingSystem", text,
			NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
		nextLine += NEXT_LINE_OFFSET;
	}
}

void Profiler::UpdateTimers()
{
	for (std::pair<std::string, GameTimer*> subsystemTimer : timers)
	{
		nextLine += NEXT_LINE_OFFSET;

		const float timeTaken = subsystemTimer.second->GetTimeTakenForSection();
		std::string profilerText = subsystemTimer.first + "	" + std::to_string(timeTaken);

		if (timeTaken > FLT_EPSILON)
		{
			profilerText += "  ---  " + std::to_string(1000.0f / timeTaken) + "fps";
		}
		
		NCLVector3 position(-500.0f, nextLine, 0);

		messages.push_back(TextMeshMessage("RenderingSystem", profilerText,
			position, TEXT_SIZE, TEXT_COLOUR, true, true));

		SaveProfilingInfo(subsystemTimer.second, 1, -500.0f);
	}
}

void Profiler::DisplayChildTimers()
{
	for (TextMeshMessage& message : messages)
	{
		DeliverySystem::GetPostman()->InsertMessage(message);
	}

	timersDisplayed = true;
	externalText.clear();
}

void Profiler::SaveProfilingInfo(GameTimer* parentTimer, int currentDepth, float parentXOffset)
{
	std::vector<GameTimer*> childTimers = parentTimer->GetAllChildTimers();

	float xOffset = parentXOffset + 20.0f;

	if (currentDepth <= depth)
	{
		for (GameTimer* childTimer : childTimers)
		{
			nextLine += NEXT_LINE_OFFSET;

			std::string profilerText = childTimer->GetTimerName() + "	" + std::to_string(
				childTimer->GetTimeTakenForSection());
			messages.push_back(TextMeshMessage("RenderingSystem", profilerText,
				NCLVector3(xOffset, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));

			SaveProfilingInfo(childTimer, ++currentDepth, xOffset);
		}
	}
}
