#pragma once

#include "../Communication/Message.h"
#include <string>
#include "../Utilities/Maths/Vector3.h"

class ApplyImpulseMessage : public Message
{
public:
	ApplyImpulseMessage(const std::string& desinationName, std::string gameObjectID, bool isRandom, 
		NCLVector3 impulse, float xmin = 0.0f, float xmax = 0.0f, float ymin = 0.0f, float ymax = 0.0f, 
		float zmin = 0.0f, float zmax = 0.0f);
	~ApplyImpulseMessage();

	static ApplyImpulseMessage builder(Node* node);
	static ApplyImpulseMessage tokensToMessage(std::vector<std::string> lineTokens);

	std::string gameObjectID;
	bool isRandom;
	NCLVector3 impulse;

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;
	
};

