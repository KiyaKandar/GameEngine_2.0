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

	static SoundNode Builder(PlaySoundMessage* message, Sound* sound);
	static SoundNode Builder(PlayMovingSoundMessage* message, Sound* sound);

	float GetRadius() const
	{
		return radius;
	}

	OALSource* GetSource() const
	{
		return oalSource;
	}

	bool HasSound() const
	{
		return sound != nullptr;
	}

	double GetTimeLeft() const
	{
		return timeLeft;
	}

	bool GetIsLooping() const
	{
		return isLooping;
	}

	NCLVector3 GetPosition() const
	{
		return position;
	}

	NCLVector3* GetMovingPosition() const
	{
		return movingPosition;
	}

	void SetMovingPosition(NCLVector3* position)
	{
		movingPosition = position;
	}

	void SetGameObject(GameObject* gObj)
	{
		this->gObj = gObj;
	}

	void SetPosition(NCLVector3 position)
	{
		this->position = position;
	}

	static bool CompareSourcesByPriority(SoundNode& a, SoundNode& b);

	void AttachSource(OALSource* s);
	void DetachSource();

	void Update(float msec);

	bool enabled = false;
	bool isMoving = false;
	bool isGlobal;
	std::string identifier;
	

	GameObject* GetObject()
	{
		return gObj;
	}

	SoundState GetState()
	{
		return state;
	}

	void PauseSound();
	void UnpauseSound();

private:
	void SetSound(Sound *s);

	void SetVolume(float volume)
	{
		this->volume = min(1.0f, max(0.0f, volume));
	}

	void SetRadius(float value)
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

