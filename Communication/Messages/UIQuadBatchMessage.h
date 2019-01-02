#pragma once

#include "../Message.h"
#include "Maths/Vector2.h"
#include "Maths/Vector3.h"

struct UIQuad
{
	NCLVector3 screenPosition;
	NCLVector3 scale;
	NCLVector3 colour;
};

struct UIQuadOrder
{
	bool operator() (const UIQuad& lhs, const UIQuad& rhs)
	{
		return lhs.screenPosition.z > rhs.screenPosition.z;
	}
};

class UIQuadBatchMessage : public Message
{
public:
	UIQuadBatchMessage() : Message("", DUMMY_TYPE) {}
	UIQuadBatchMessage(const std::string &destinationName, const std::vector<UIQuad>* uiQuads);
	~UIQuadBatchMessage();

	const std::vector<UIQuad>* uiQuads;
};

