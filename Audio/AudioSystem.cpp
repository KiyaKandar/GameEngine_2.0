#include "AudioSystem.h"

#include "../Communication/Messages/PlaySoundMessage.h"
#include "../Utilities/GameTimer.h"

AudioSystem::AudioSystem(Database *database, Camera *camera) : Subsystem("AudioSystem")
{
	soundManager = std::make_unique<SoundManager>(database, camera);

	incomingMessages = MessageProcessor(std::vector<MessageType>{ MessageType::PLAY_SOUND, MessageType::STOP_SOUND, MessageType::MOVING_SOUND }, 
		DeliverySystem::GetPostman()->GetDeliveryPoint("AudioSystem"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::PLAY_SOUND, [&soundManagerPointer = soundManager](Message* message)
	{
		PlaySoundMessage* soundMessage = static_cast<PlaySoundMessage*>(message);
		soundManagerPointer->AddNewSoundNode(soundMessage);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::MOVING_SOUND, [&soundManagerPointer = soundManager](Message* message)
	{
		PlayMovingSoundMessage* soundMessage = static_cast<PlayMovingSoundMessage*>(message);
		soundManagerPointer->AddNewSoundNode(soundMessage);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::STOP_SOUND, [&soundManagerPointer = soundManager](Message* message)
	{
		StopSoundMessage* soundMessage = static_cast<StopSoundMessage*>(message);
		soundManagerPointer->StopSoundNode(soundMessage);
	});
}

void AudioSystem::UpdateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	soundManager->Update(deltaTime);

	timer->endTimedSection();
}
