#pragma once

#include <string>
#include "InputFilter.h"

class GameObject;
class InputRecorder;

class Player
{

public:
	Player() = default;
	Player(int playerID, InputRecorder *recorder);
	~Player();

	void setPlayerID(int newID);
	const int getPlayerID();

	void setInputRecorder(InputRecorder* newInputRecorder);
	InputRecorder* getInputRecorder();

	void setPlayerControls(InputFilter* newControls);
	InputFilter* getInputFilter();

	void setGameObject(GameObject* node);
	GameObject* getGameObject();

private:
	GameObject* gameObject;
	int playerID;
	InputRecorder *recorder = nullptr;
	InputFilter playerControls;


};

