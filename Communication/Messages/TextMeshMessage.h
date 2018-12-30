#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Vector3.h"

class TextMeshMessage : public Message
{
public:
	TextMeshMessage() : Message("", DUMMY_TYPE) {}
	TextMeshMessage(const std::string& destinationName, const std::string& text, 
		NCLVector3 position, NCLVector3 scale, NCLVector3 colour, bool orthographic, bool hasBackground = false);
	~TextMeshMessage();

	static TextMeshMessage Builder(Node* node);

	bool hasBackground;
	bool orthographic;
	std::string text;
	NCLVector3 position;
	NCLVector3 scale;
	NCLVector3 colour;
};

