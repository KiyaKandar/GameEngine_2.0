#pragma once

#include "../Message.h"

class TextMessage : public Message
{
public:
	TextMessage(const std::string& destionation, const std::string text);
	~TextMessage();

	std::string text;
};

