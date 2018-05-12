#pragma once

#include <string>
#include "../Utilities/Maths/Vector3.h"

class Text
{
public:

	Text(std::string text = "", NCLVector3 position = NCLVector3(),
		float size = 0, bool perspective = false)
		:text(text),
		position(position),
		size(size),
		perspective(perspective) {}

	~Text() {}

	std::string text;
	NCLVector3 position;
	float size;
	bool perspective;
};