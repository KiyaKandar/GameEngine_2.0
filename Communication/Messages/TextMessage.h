#pragma once

#include "../Message.h"

class TextMessage : public Message
{
public:
	TextMessage() : Message("", DUMMY_TYPE) {}
	TextMessage(const std::string& destionation, const std::string text);
	~TextMessage();

	std::string text;
};

