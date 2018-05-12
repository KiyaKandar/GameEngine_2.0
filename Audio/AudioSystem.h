#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "SoundManager.h"

#include <memory>

class AudioSystem : public Subsystem
{
public:
	AudioSystem(Database *database, Camera *camera);
	~AudioSystem() = default;

	void updateNextFrame(const float& deltaTime) override;

	void clearSoundNodesWhenUnloadingLevel()
	{
		soundManager->clearSoundNodes();
	}

private:
	std::unique_ptr<SoundManager> soundManager;
};

