#pragma once
#include "../Utilities/Maths/Vector2.h"
#include "../Players/PlayerBase.h"
#include <vector>
#include <unordered_map>

enum InputType {
	TRIGGERED,HELD,RELEASED, NONE
};

//remove initialiser lists. 
//Move brackets to next line etc
struct ButtonInputData {
	ButtonInputData(const InputType currentState, const unsigned int key)
	{
		this->currentState = currentState;
		this->key = key;
	}

	ButtonInputData()
	{
		this->currentState = NONE;
		this->key = 0;
	}

	InputType currentState;
	unsigned int key;
};

struct LinearInputData {
	LinearInputData(const float value, const std::string key)
	{
		this->value = value;
		this->key = key;	
	}
	float value;
	std::string key;

};

struct PositionalInputData {
	PositionalInputData(const NCLVector2 value, const std::string key)
	{
		this->value = value;
		this->key = key;
	}
	NCLVector2 value;
	std::string key;
};

class InputRecorder
{
public:
	explicit InputRecorder(std::vector<int> keysToListen) {};
	InputRecorder() {}
	~InputRecorder() {}

	virtual void FillInputs() = 0;
	virtual void ClearInputs();

	std::vector<ButtonInputData> const GetInputs();
	std::vector<int> const GetKeysToListen();
	
	void AddKeysToListen(std::vector<int> keysToListen);
	void RemoveListenedKey(int key);//TODO
	void AddKeyToListen(int key);

protected:
	std::vector<ButtonInputData> currentButtonInputs;
	std::vector<LinearInputData> currentLinearInputs;
	std::vector<PositionalInputData> currentPositionalInputs;

	std::vector<int> keysToListen;
	PlayerBase* player;
};

