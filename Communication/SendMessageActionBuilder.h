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
	static void initialiseBuilders();
	static Executable buildSendMessageAction(Node* node);
	static Executable buildSendMessageAction(std::string devConsoleLine);

private:
	static void initialiseNodeBuilders();

	template <class T>
	inline static Executable buildExecutable(Node* node, std::function<TrackedMessageSender<T>**()> addSender)
	{
		T message = T::builder(node);

		if (node->children[0] != nullptr && node->children[0]->nodeType == "Tracked")
		{
			TrackedMessageSender<T>** sender = addSender();
			(*sender)->setMessage(message);

			return[sender = *sender]()
			{
				if (sender->readyToSendNextMessage())
				{
					sender->sendMessage();
				}
			};
		}

		return [message = message]()
		{
			DeliverySystem::getPostman()->insertMessage(message);
		};
	}

	static void initialiseDevConsoleBuilders();

	static std::unordered_map<std::string, Builder> builders;
	static std::unordered_map<std::string, DevConsoleNodeBuilder> devConsoleBuilder;
};