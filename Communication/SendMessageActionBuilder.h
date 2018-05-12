#pragma once

#include <functional>
#include <unordered_map>

struct Node;

typedef std::function<void()> Executable;
typedef std::function<Executable(Node*)> Builder;
typedef std::function<Executable(std::vector<std::string>)> DevConsoleNodeBuilder;

class SendMessageActionBuilder
{
public:
	static void initialiseBuilders();
	static Executable buildSendMessageAction(Node* node);
	static Executable buildSendMessageAction(std::string devConsoleLine);

private:
	static void initialiseNodeBuilders();
	static void initialiseDevConsoleBuilders();

	static std::unordered_map<std::string, Builder> builders;
	static std::unordered_map<std::string, DevConsoleNodeBuilder> devConsoleBuilder;
};

