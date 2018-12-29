#pragma once

#include "../Message.h"
#include "../../Physics/CollisionDetectionSAT.h"

class CollisionMessage : public Message
{
public:
	CollisionMessage() : Message("", DUMMY_TYPE) {}
	CollisionMessage(const std::string& destinationName, CollisionData collisionData,
		std::string objectIdentifier, std::string colliderIdentifier);
	~CollisionMessage();

	CollisionData collisionData;
	std::string objectIdentifier;
	std::string colliderIdentifier;
};

