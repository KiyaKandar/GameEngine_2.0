#include "AbsoluteTransformMessage.h"

AbsoluteTransformMessage::AbsoluteTransformMessage(const std::string& destinationName, const std::string& resourceName,
	NCLMatrix4 transform)
	: Message(destinationName, ABSOLUTE_TRANSFORM)
{
	this->transform = transform;
	this->resourceName = resourceName;
}

AbsoluteTransformMessage::~AbsoluteTransformMessage()
{
}

AbsoluteTransformMessage AbsoluteTransformMessage::Builder(Node* node)
{
	std::string nodeDestination = "";
	std::string nodeResourcename = "";
	NCLVector3 nodeTranslation(0, 0, 0);
	NCLVector4 nodeRotation(0, 0, 0, 0);
	NCLVector3 nodeScale(1, 1, 1);

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			nodeDestination = childNode->value;
		}
		else if (childNode->nodeType == "translation")
		{
			nodeTranslation = VectorBuilder::BuildVector3(childNode);
		}
		else if (childNode->nodeType == "rotation")
		{
			nodeRotation = VectorBuilder::BuildVector4(childNode);
		}
		else if (childNode->nodeType == "scale")
		{
			nodeScale = VectorBuilder::BuildVector3(childNode);
		}
		else if (childNode->nodeType == "resource")
		{
			nodeResourcename = childNode->value;
		}
	}

	NCLMatrix4 nodeTransform = NCLMatrix4::Translation(nodeTranslation)
		* NCLMatrix4::Rotation(nodeRotation.w, NCLVector3(nodeRotation.x, nodeRotation.y, nodeRotation.z))
		* NCLMatrix4::Scale(nodeScale);

	return AbsoluteTransformMessage(nodeDestination, nodeResourcename, nodeTransform);
}

AbsoluteTransformMessage AbsoluteTransformMessage::TokensToMessage(std::vector<std::string> lineTokens)
{
	std::string nodeDestination = lineTokens[1];
	std::string nodeResourcename = lineTokens[2];
	NCLVector3 nodeTranslation = VectorBuilder::BuildVector3(lineTokens[3].substr(12));
	NCLVector4 nodeRotation = VectorBuilder::BuildVector4(lineTokens[4].substr(9));
	NCLVector3 nodeScale = VectorBuilder::BuildVector3(lineTokens[5].substr(6));

	NCLMatrix4 nodeTransform = NCLMatrix4::Translation(nodeTranslation)
		* NCLMatrix4::Rotation(nodeRotation.w, NCLVector3(nodeRotation.x, nodeRotation.y, nodeRotation.z))
		* NCLMatrix4::Scale(nodeScale);

	return AbsoluteTransformMessage(nodeDestination, nodeResourcename, nodeTransform);
}
