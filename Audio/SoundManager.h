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

	void Update(const float& deltaTime);
	void AddNewSoundNode(PlaySoundMessage* message);
	void AddNewSoundNode(PlayMovingSoundMessage* message);
	void StopSoundNode(StopSoundMessage* message);

	void ClearSoundNodes();

private:
	void InitialiseOpenAl();
	void createOALSources();

	OALSource* GetOalSource();
	
	void UpdateListenerToCameraPosition();
	void CullNodes();
	void DetachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to);
	void AttachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to);
	void RemoveSoundNodesFromSystem();

	ALCcontext* context;
	ALCdevice* device;

	//make this a set to avoid duplicates
	std::vector<SoundNode> soundNodes;
	std::vector<OALSource*> OALSources;

	Camera* camera;
	Database *database;

	NCLVector3 listenerPosition;
};

