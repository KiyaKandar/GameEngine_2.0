#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Vector3.h"

class TextMeshMessage : public Message
{
public:
	TextMeshMessage(const std::string& destinationName, const std::string& text, 
		NCLVector3 position, NCLVector3 scale, NCLVector3 colour, bool orthographic, bool hasBackground = false);
	~TextMeshMessage();

	static TextMeshMessage builder(Node* node);

	bool hasBackground;
	bool orthographic;
	std::string text;
	NCLVector3 position;
	NCLVector3 scale;
	NCLVector3 colour;
};

