#include "CollisionMessage.h"

CollisionMessage::CollisionMessage(const std::string& destinationName, CollisionData collisionData,
	std::string objectIdentifier, std::string colliderIdentifier)
	: Message(destinationName, COLLISION)
{
	this->collisionData = collisionData;
	this->objectIdentifier = objectIdentifier;
	this->colliderIdentifier = colliderIdentifier;

	integerInformation.insert({ "objectIdentifier", objectIdentifier });
	integerInformation.insert({ "colliderIdentifier", colliderIdentifier });
}

CollisionMessage::~CollisionMessage()
{
}
