#include "InputManager.h"

#include "Recorders/InputRecorder.h"
#include "../Communication/DeliverySystem.h"
#include "Communication/Messages/PlayerInputMessage.h"
#include "Communication/LetterBox.h"
#include "../Gameplay/GameObject.h"
#include "../Physics/PhysicsNode.h"
#include <iterator>

InputManager::InputManager(PlayerBase* playerbase)
	: Subsystem("InputManager")
{
	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::TEXT },
		DeliverySystem::GetPostman()->GetDeliveryPoint("InputManager"));

	inputControl.RegisterNewInputUserByDeliveryPoint("InputManager");

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [&inputControl = inputControl, &blocked = blocked](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "RegisterInputUser")
		{
			inputControl.RegisterNewInputUserByDeliveryPoint(tokens[1]);
		}
		else if (tokens[0] == "BlockAllInputs")
		{
			inputControl.BlockAllInputUsersOtherThanCaller(tokens[1]);
		}
		else if (tokens[0] == "UnblockAll")
		{
			inputControl.UnlockBlockedUsers();
		}
		else
		{
			blocked = InputControl::IsBlocked(textMessage->text);
		}
	});

	blocked = false;
	this->playerbase = playerbase;
}

InputManager::~InputManager()
{
	delete playerbase;
}

void InputManager::UpdateNextFrame(const float& deltatime)
{
	if (!blocked)
	{
		timer->BeginTimedSection();

		for (Player* player : playerbase->GetPlayers())
		{
			player->getInputRecorder()->ClearInputs();
			player->getInputRecorder()->FillInputs();

			std::vector<ButtonInputData> inputData = player->getInputRecorder()->GetInputs();

			SendInputMessagesForKeys(inputData, player);
		}

		timer->EndTimedSection();
	}
}

void InputManager::SendInputMessagesForKeys(std::vector<ButtonInputData>& inputData, Player* player)
{
	if (inputMessageSender.ReadyToSendNextMessageGroup())
	{
		std::vector<PlayerInputMessage> messages;

		for (int i = 0; i < inputData.size(); ++i)
		{
			messages.push_back(PlayerInputMessage("Gameplay", player, inputData[i]));
		}

		inputMessageSender.SetMessageGroup(messages);
		inputMessageSender.SendMessageGroup();
	}
}