#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "SoundManager.h"

#include <memory>

class AudioSystem : public Subsystem
{
public:
	AudioSystem(Database *database, Camera *camera);
	~AudioSystem() = default;

	void UpdateNextFrame(const float& deltaTime) override;

	void ClearSoundNodesWhenUnloadingLevel() const
	{
		soundManager->ClearSoundNodes();
	}

private:
	std::unique_ptr<SoundManager> soundManager;
};

