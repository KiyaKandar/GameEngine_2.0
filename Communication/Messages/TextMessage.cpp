#include "TextMessage.h"


TextMessage::TextMessage(const std::string& destination, const std::string text) : Message(destination, TEXT)
{
	this->text = text;
}

TextMessage::~TextMessage()
{
}
