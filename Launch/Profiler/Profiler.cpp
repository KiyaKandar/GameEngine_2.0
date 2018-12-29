#include "Profiler.h"

#include "../Resource Management/Database/Database.h"
#include "../Utilities/GameTimer.h"
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

	incomingMessages = MessageProcessor::MessageProcessor(std::vector<MessageType>{MessageType::TEXT}, DeliverySystem::getPostman()->getDeliveryPoint("Profiler"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [&externalText = externalText](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		externalText.push_back(textMessage->text);
	});

	memoryWatcher = MemoryWatcher(database->MaxSize(), database);

	f5Listener = SinglePressKeyListener(KEYBOARD_F5, keyboard);
	f6Listener = SinglePressKeyListener(KEYBOARD_F6, keyboard);
}

void Profiler::updateNextFrame(const float& deltatime)
{
	fpsCounter->calculateFPS(deltatime);

	if (f5Listener.keyPressed())
	{
		profilerEnabled = !profilerEnabled;
		depth = 0;
	}

	if (f6Listener.keyPressed())
	{
		++depth;
	}

	if (profilerEnabled)
	{
		++numFramesUpdated;

		if (numFramesUpdated >= 30)
		{
			numFramesUpdated = 0;
			messages.clear();

			memoryWatcher.Update();
			updateProfiling();
		}

		displayChildTimers();
	}
}

void Profiler::addSubsystemTimer(string name, GameTimer* timer)
{
	timers.insert({ name, timer });
}

void Profiler::updateProfiling()
{
	nextLine = Y_OFFSET;

	if (timersDisplayed)
	{

		updateFPS();
		updateMemory();
		updateTimers();

		for (std::string text : externalText)
		{
			messages.push_back(TextMeshMessage("RenderingSystem", text,
				NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
			nextLine += NEXT_LINE_OFFSET;
		}

		timersDisplayed = false;
	}
}

void Profiler::updateFPS()
{
	TextMeshMessage message("RenderingSystem", "FPS : " + std::to_string(fpsCounter->fps),
		NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true);

	messages.push_back(message);
	nextLine += NEXT_LINE_OFFSET;
}

void Profiler::updateMemory()
{
	messages.push_back(TextMeshMessage("RenderingSystem", "Memory Usage : " + std::to_string(memoryWatcher.percent),
		NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
	nextLine += NEXT_LINE_OFFSET;

	for (size_t i = 0; i < database->getAllTables().size(); i++)
	{
		std::string text = database->getAllTables()[i]->getName() + ": " +
			std::to_string(database->getAllTables()[i]->getAllResources()->getCurrentSize());
		messages.push_back(TextMeshMessage("RenderingSystem", text,
			NCLVector3(-500.0f, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));
		nextLine += NEXT_LINE_OFFSET;
	}
}

void Profiler::updateTimers()
{
	for (std::pair<std::string, GameTimer*> subsystemTimer : timers)
	{
		nextLine += NEXT_LINE_OFFSET;

		std::string profilerText = subsystemTimer.first + "	" + std::to_string(subsystemTimer.second->getTimeTakenForSection());
		NCLVector3 position(-500.0f, nextLine, 0);

		messages.push_back(TextMeshMessage("RenderingSystem", profilerText,
			position, TEXT_SIZE, TEXT_COLOUR, true, true));

		saveProfilingInfo(subsystemTimer.second, 1, -500.0f);
	}
}

void Profiler::displayChildTimers()
{
	if (profilerTextSender.readyToSendNextMessageGroup())
	{
		profilerTextSender.setMessageGroup(messages);
		profilerTextSender.sendMessageGroup();
		timersDisplayed = true;
		externalText.clear();
	}
}

void Profiler::saveProfilingInfo(GameTimer* parentTimer, int currentDepth, float parentXOffset)
{
	std::vector<GameTimer*> childTimers = parentTimer->getAllChildTimers();

	float xOffset = parentXOffset + 20.0f;

	if (currentDepth <= depth)
	{
		for (GameTimer* childTimer : childTimers)
		{
			nextLine += NEXT_LINE_OFFSET;

			std::string profilerText = childTimer->getTimerName() + "	" + std::to_string(childTimer->getTimeTakenForSection());
			messages.push_back(TextMeshMessage("RenderingSystem", profilerText,
				NCLVector3(xOffset, nextLine, 0), TEXT_SIZE, TEXT_COLOUR, true, true));

			saveProfilingInfo(childTimer, ++currentDepth, xOffset);
		}
	}
}
