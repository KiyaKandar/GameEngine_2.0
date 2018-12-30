#pragma once

#include <string>
#include <unordered_map>
#include <functional>

struct Node;
struct Button;

typedef std::function<void()> ButtonAction;

class ButtonActionCreator
{
public:
	ButtonActionCreator();
	~ButtonActionCreator();

	ButtonAction CreateButtonAction(Node* actionNode);

private:
	std::unordered_map<std::string, ButtonAction> actions;
};

