#pragma once

#include <string>

class PeriodicTextModifier
{
public:
	PeriodicTextModifier() = default;
	PeriodicTextModifier(std::string baseText, std::string textToPeriodicallyAdd, int maximumAdditions);
	~PeriodicTextModifier();

	void AddTextWhenTimeHasReachedMaximum(const int maximumFramesUntilAddition);
	std::string GetCurrentString() const;

private:
	std::string baseText; 
	std::string currentText;
	std::string textToPeriodicallyAdd; 
	int maximumAdditions;
	int currentNumberOfAdditions;
	int currentFrameCount;
};

