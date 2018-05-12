#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Vector3.h"

class MoveCameraRelativeToGameObjectMessage : public Message
{
public:
	MoveCameraRelativeToGameObjectMessage(const std::string& destinationName, const std::string& resourceName,
		NCLVector3 translation, float pitch, float yaw);
	~MoveCameraRelativeToGameObjectMessage();

	static std::string resourceName;
	NCLVector3 translation;
	float pitch;
	float yaw;

	static MoveCameraRelativeToGameObjectMessage builder(Node* node);
};

