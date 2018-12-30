#include "SoundManager.h"

#include "../Resource Management/Database/Database.h"
#include "../Communication/Messages/PlaySoundMessage.h"
#include "../Communication/Messages/PlayMovingSoundMessage.h"
#include "../Communication/Messages/StopSoundMessage.h"
#include "../Graphics/Utility/Camera.h"
#include "../Gameplay/GameObject.h"


const int CHANNELS = 128;
const int FORWARD_DIRECTION = 0;
const int UPWARDS_DIRECTION = 1;

SoundManager::SoundManager(Database* database, Camera* camera)
{
	InitialiseOpenAl();
	createOALSources();

	this->database = database;
	this->camera = camera;
}

SoundManager::~SoundManager()
{
	alcMakeContextCurrent(NULL);

	for (vector<OALSource*>::iterator i = OALSources.begin(); i != OALSources.end(); ++i)
	{
		delete *i;
		*i = nullptr;
		alDeleteSources(1, &(*i)->source);
	}

	alcDestroyContext(context);
	alcCloseDevice(device);
}

void SoundManager::InitialiseOpenAl()
{
	device = alcOpenDevice(NULL);

	if (!device)
	{
		throw runtime_error("No OALDevice");
	}

	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
}

void SoundManager::createOALSources()
{
	for (unsigned int i = 0; i < CHANNELS; ++i)
	{
		ALuint source;
		alGenSources(1, &source);
		const ALenum error = alGetError();

		if (error == AL_NO_ERROR)
		{
			OALSources.push_back(new OALSource(source));
		}
		else
		{
			break;
		}
	}
}

void SoundManager::AddNewSoundNode(PlaySoundMessage* message)
{
	if (!soundNodes.empty())
	{
		std::string tempID = "";

		for (unsigned int i = 0; i < soundNodes.size(); ++i)
		{
			if (soundNodes[i].identifier == message->soundNodeIdentifier)
			{
				tempID = soundNodes[i].identifier;
				break;
			}
		}
		if (tempID != message->soundNodeIdentifier)
		{
			Sound* sound = static_cast<Sound*>(database->GetTable("SoundObjects")->GetResource(message->soundID));
			soundNodes.push_back(SoundNode::Builder(message, sound));
		}
	}
	else
	{
		Sound* sound = static_cast<Sound*>(database->GetTable("SoundObjects")->GetResource(message->soundID));
		soundNodes.push_back(SoundNode::Builder(message, sound));
	}
}

void SoundManager::AddNewSoundNode(PlayMovingSoundMessage* message)
{
	if (!soundNodes.empty())
	{
		std::string tempID = "";

		for (unsigned int i = 0; i < soundNodes.size(); ++i)
		{
			if (soundNodes[i].identifier == message->soundNodeIdentifier)
			{
				tempID = soundNodes[i].identifier;
				break;
			}
		}
		if (tempID != message->soundNodeIdentifier)
		{
			Sound* sound = static_cast<Sound*>(database->GetTable("SoundObjects")->GetResource(message->soundID));
			soundNodes.push_back(SoundNode::Builder(message, sound));
			if (message->isGlobal)
			{
				soundNodes.back().SetMovingPosition(camera->GetPersistentPosition());
			}
			else
			{
				GameObject* gObj =  static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(message->gameObjectID));
				soundNodes.back().SetGameObject(gObj);
				soundNodes.back().SetPosition(gObj->GetPosition());
			}
		}
	}
	else
	{
		Sound* sound = static_cast<Sound*>(database->GetTable("SoundObjects")->GetResource(message->soundID));
		soundNodes.push_back(SoundNode::Builder(message, sound));
		if (message->isGlobal)
		{
			soundNodes.back().SetMovingPosition(camera->GetPersistentPosition());
		}
		else
		{
			GameObject* gObj = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(message->gameObjectID));
			soundNodes.back().SetGameObject(gObj);
			soundNodes.back().SetPosition(gObj->GetPosition());
		}
	}
}

void SoundManager::StopSoundNode(StopSoundMessage* message)
{
	for (unsigned int i = 0; i < soundNodes.size(); ++i)
	{
		if (soundNodes[i].identifier == message->soundNodeIdentifier)
		{
			soundNodes[i].enabled = false;
			soundNodes[i].DetachSource();
		}
	}
}


void SoundManager::Update(const float& deltaTime)
{
	UpdateListenerToCameraPosition();

	for (SoundNode& node : soundNodes)
	{
		if (deltaTime == 0.0f)
		{
			node.PauseSound();
		}
		else
		{
			if(node.GetState() == SoundState::PAUSED)
			{
				node.UnpauseSound();
			}
			node.Update(deltaTime);
		}
	}

	CullNodes();

	if (soundNodes.size() <= OALSources.size())
	{
		AttachSources(soundNodes.begin(), soundNodes.end());
	}
	else
	{
		std::sort(soundNodes.begin(), soundNodes.end(), SoundNode::CompareSourcesByPriority);
		DetachSources((soundNodes.begin() + (OALSources.size() + 1)), soundNodes.end());
		AttachSources((soundNodes.begin() + (OALSources.size() + 1)), soundNodes.end());
	}

	RemoveSoundNodesFromSystem();
}

void SoundManager::UpdateListenerToCameraPosition()
{
	listenerPosition = camera->GetPosition();

	NCLVector3 orientation[2];

	orientation[FORWARD_DIRECTION].x = camera->viewMatrix.values[2];
	orientation[FORWARD_DIRECTION].y = camera->viewMatrix.values[6];
	orientation[FORWARD_DIRECTION].z = camera->viewMatrix.values[10];

	orientation[UPWARDS_DIRECTION].x = camera->viewMatrix.values[1];
	orientation[UPWARDS_DIRECTION].y = camera->viewMatrix.values[5];
	orientation[UPWARDS_DIRECTION].z = camera->viewMatrix.values[9];

	ALfloat listenerPos[] = { listenerPosition.x, listenerPosition.y, listenerPosition.z  };
	ALfloat listenerOri[] = { 
		orientation[FORWARD_DIRECTION].x, orientation[FORWARD_DIRECTION].y, -orientation[FORWARD_DIRECTION].z, 
		orientation[UPWARDS_DIRECTION].x, orientation[UPWARDS_DIRECTION].y, -orientation[UPWARDS_DIRECTION].z };

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_ORIENTATION, listenerOri);
}

void SoundManager::CullNodes()
{
	for (SoundNode& node : soundNodes)
	{
		float distanceBetweenListenerAndSoundNode;

		if(!node.isMoving || (node.isMoving && !node.isGlobal))
		{
			distanceBetweenListenerAndSoundNode = (listenerPosition - node.GetPosition()).length();
		}
		else if (node.isMoving && node.isGlobal)
		{
			distanceBetweenListenerAndSoundNode = (listenerPosition - *node.GetMovingPosition()).length();
		}

		if (distanceBetweenListenerAndSoundNode > node.GetRadius() || !node.HasSound() || node.GetTimeLeft() < 0)
		{
			if (node.GetIsLooping() || node.GetTimeLeft() > 0)
			{
				node.DetachSource();
			}
			else
			{
				node.enabled = false;
				node.DetachSource();
			}
		}
	}
}

void SoundManager::DetachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to)
{
	for (std::vector<SoundNode>::iterator i = from; i != to; ++i)
	{
		(*i).DetachSource();
	}
}

void SoundManager::AttachSources(std::vector<SoundNode>::iterator& from, std::vector<SoundNode>::iterator& to)
{
	for (vector<SoundNode>::iterator i = from; i != to; ++i)
	{
		if (!(*i).GetSource() && (*i).enabled)
		{
			(*i).AttachSource(GetOalSource());
		}
	}
}

OALSource* SoundManager::GetOalSource()
{
	for (vector<OALSource*>::iterator i = OALSources.begin(); i != OALSources.end(); ++i)
	{
		if (!(*i)->inUse)
		{
			return (*i);
		}
	}

	return nullptr;
}

void SoundManager::RemoveSoundNodesFromSystem()
{
	for (vector<SoundNode>::iterator i = soundNodes.begin(); i != soundNodes.end();)
	{
		if(!i->enabled && !i->GetSource())
		{
			i = soundNodes.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void SoundManager::ClearSoundNodes()
{
	for (SoundNode& s : soundNodes)
	{
		s.enabled = false;
		s.DetachSource();
	}

	RemoveSoundNodesFromSystem();
}


