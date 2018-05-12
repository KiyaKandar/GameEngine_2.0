#pragma once

#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Matrix4.h"
#include "Sound.h"
#include "../Gameplay/GameObject.h"

enum SoundState
{
	PLAYING,
	PAUSED
};

class PlaySoundMessage;
class PlayMovingSoundMessage;
class StopSoundMessage;

enum SoundPriority 
{
	SOUNDPRIORITY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

struct OALSource
{
	ALuint source;
	bool inUse;
	
	OALSource(ALuint src) 
	{
		source = src;
		inUse = false;
	}
};

class SoundNode
{
public:
	SoundNode(Sound* sound, NCLVector3 position, SoundPriority priority, float volume, 
		bool isLooping, float radius, float pitch, std::string identifier);
	SoundNode(Sound* sound, NCLVector3 *position, SoundPriority priority, float volume,
		bool isLooping, float radius, float pitch, bool isGlobal, std::string identifier);
	~SoundNode();

	static SoundNode builder(PlaySoundMessage* message, Sound* sound);
	static SoundNode builder(PlayMovingSoundMessage* message, Sound* sound);

	float getRadius() const
	{
		return radius;
	}

	OALSource* getSource() const
	{
		return oalSource;
	}

	bool hasSound() const
	{
		return sound != nullptr;
	}

	double getTimeLeft() const
	{
		return timeLeft;
	}

	bool getIsLooping() const
	{
		return isLooping;
	}

	NCLVector3 getPosition() const
	{
		return position;
	}

	NCLVector3* getMovingPosition() const
	{
		return movingPosition;
	}

	void setMovingPosition(NCLVector3* position)
	{
		movingPosition = position;
	}

	void setGameObject(GameObject* gObj)
	{
		this->gObj = gObj;
	}

	void setPosition(NCLVector3 position)
	{
		this->position = position;
	}

	static bool compareSourcesByPriority(SoundNode& a, SoundNode& b);

	void attachSource(OALSource* s);
	void detachSource();

	void update(float msec);

	bool enabled = false;
	bool isMoving = false;
	bool isGlobal;
	std::string identifier;
	

	GameObject* getObject()
	{
		return gObj;
	}

	SoundState getState()
	{
		return state;
	}

	void pauseSound();
	void unpauseSound();

private:
	void setSound(Sound *s);

	void setVolume(float volume)
	{
		this->volume = min(1.0f, max(0.0f, volume));
	}

	void setRadius(float value)
	{
		radius = max(0.0f, value);
	}

	Sound* sound;
	NCLVector3 position;
	NCLVector3 *movingPosition;
	OALSource* oalSource;
	SoundPriority priority;
	float volume;
	float radius;
	float pitch;
	bool isLooping;
	double timeLeft;
	SoundState state;

	GameObject* gObj;
};

