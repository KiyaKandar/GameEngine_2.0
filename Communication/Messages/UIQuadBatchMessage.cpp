#include "UIQuadBatchMessage.h"

UIQuadBatchMessage::UIQuadBatchMessage(const std::string &destinationName, const std::vector<UIQuad>* uiQuads)
	: Message(destinationName, UIQUAD)
	, uiQuads(uiQuads)
{
}

UIQuadBatchMessage::~UIQuadBatchMessage()
{
}
