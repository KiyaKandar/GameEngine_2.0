#pragma once

#include "MessageSenders/TrackedMessageSender.h"
#include "DeliverySystem.h"

#include <functional>
#include <unordered_map>

struct Node;

typedef std::function<void()> Executable;
typedef std::function<Executable(Node*)> Builder;
typedef std::function<Executable(std::vector<std::string>)> DevConsoleNodeBuilder;

#include "../Resource Management/XMLParser.h"

class SendMessageActionBuilder
{
public:
	static void InitialiseBuilders();
	static Executable BuildSendMessageAction(Node* node);
	static Executable BuildSendMessageAction(std::string devConsoleLine);

private:
	static void InitialiseNodeBuilders();

	template <class T>
	inline static Executable BuildExecutable(Node* node, std::function<TrackedMessageSender<T>**()> addSender)
	{
		T message = T::Builder(node);

		if (node->children[0] != nullptr && node->children[0]->nodeType == "Tracked")
		{
			TrackedMessageSender<T>** sender = addSender();
			(*sender)->SetMessage(message);

			return[sender = *sender]()
			{
				if (sender->ReadyToSendNextMessage())
				{
					sender->SendTrackedMessage();
				}
			};
		}

		return [message = message]()
		{
			DeliverySystem::GetPostman()->InsertMessage(message);
		};
	}

	static void InitialiseDevConsoleBuilders();

	static std::unordered_map<std::string, Builder> builders;
	static std::unordered_map<std::string, DevConsoleNodeBuilder> devConsoleBuilder;
};