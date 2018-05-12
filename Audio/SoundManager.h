#pragma once

#include "SoundNode.h"
#include "OpenAL 1.1 SDK\include\alc.h"

#include <vector>

class Database;
class Camera;

class SoundManager
{
public:
	SoundManager() = default;
	SoundManager(Database *database, Camera *camera);
	~SoundManager();

	void update(const float& deltaTime);
	void AddNewSoundNode(PlaySoundMessage* message);
	void AddNewSoundNode(PlayMovingSoundMessage* message);
	void stopSoundNode(StopSoundMessage* message);

	void clearSoundNodes();

private:
	void initialiseOpenAl();
	void createOALSources();

	OALSource* getOALSource();
	
	void updateListenerToCameraPosition();
	void cullNodes();
	void detachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to);
	void attachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to);
	void removeSoundNodesFromSystem();

	ALCcontext* context;
	ALCdevice* device;

	//make this a set to avoid duplicates
	std::vector<SoundNode> soundNodes;
	std::vector<OALSource*> OALSources;

	Camera* camera;
	Database *database;

	NCLVector3 listenerPosition;
};

