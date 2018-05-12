#include "TextMeshMessage.h"

TextMeshMessage::TextMeshMessage(const std::string& destinationName, const std::string& text,
	NCLVector3 position, NCLVector3 scale, NCLVector3 colour, bool orthographic, bool hasBackground)
	: Message(destinationName, TEXT_MESH_MESSAGE)
{
	this->hasBackground = hasBackground;
	this->orthographic = orthographic;
	this->text = text;
	this->position = position;
	this->scale = scale;
	this->colour = colour;
}

TextMeshMessage::~TextMeshMessage()
{
}

TextMeshMessage TextMeshMessage::builder(Node* node)
{
	std::string nodeDestination = "";
	bool nodeHasBackground = false;
	bool nodeOrthographic = false;
	std::string nodeText = "";
	NCLVector3 nodeTextPosition = NCLVector3();
	NCLVector3 nodeTextScale = NCLVector3();
	NCLVector3 nodeTextColour = NCLVector3();

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			nodeDestination = childNode->value;
		}
		else if (childNode->nodeType == "hasBackground")
		{
			nodeHasBackground = childNode->value == "True";
		}
		else if (childNode->nodeType == "orthographic")
		{
			nodeOrthographic = childNode->value == "True";
		}
		else if (childNode->nodeType == "text")
		{
			nodeText = childNode->value;
		}
		else if (childNode->nodeType == "position")
		{
			nodeTextPosition = VectorBuilder::buildVector3(childNode);
		}
		else if (childNode->nodeType == "scale")
		{
			nodeTextScale = VectorBuilder::buildVector3(childNode);
		}
		else if (childNode->nodeType == "colour")
		{
			nodeTextColour = VectorBuilder::buildVector3(childNode);
		}
	}

	return TextMeshMessage(nodeDestination, nodeText, nodeTextPosition, nodeTextScale, nodeTextColour, nodeOrthographic, nodeHasBackground);
}
