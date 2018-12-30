#include "InputRecorder.h"

void InputRecorder::ClearInputs()
{
	currentButtonInputs.clear();
	currentLinearInputs.clear();
	currentPositionalInputs.clear();
}

std::vector<ButtonInputData> const InputRecorder::GetInputs()
{
	return currentButtonInputs;
}

std::vector<int> const InputRecorder::GetKeysToListen()
{
	return keysToListen;
}

void InputRecorder::AddKeysToListen(std::vector<int> keysToListen)
{
	this->keysToListen = keysToListen;
}

void InputRecorder::RemoveListenedKey(int key)
{
	for (auto i = keysToListen.begin(); i!=keysToListen.end(); ++i)
	{
		if ((*i) == key) 
		{
			i = keysToListen.erase(i);
		}
	}
}

void InputRecorder::AddKeyToListen(int key)
{
	this->keysToListen.push_back(key);
}
