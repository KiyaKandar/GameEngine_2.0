#include "PeriodicTextModifier.h"

PeriodicTextModifier::PeriodicTextModifier(std::string baseText, std::string textToPeriodicallyAdd,
	int maximumAdditions)
{
	this->baseText = baseText;
	this->textToPeriodicallyAdd = textToPeriodicallyAdd;
	this->maximumAdditions = maximumAdditions;

	currentFrameCount = 0;
	currentNumberOfAdditions = 0;
	currentText = baseText;
}

PeriodicTextModifier::~PeriodicTextModifier()
{
}

void PeriodicTextModifier::AddTextWhenTimeHasReachedMaximum(const int maximumFramesUntilAddition)
{
	if (currentFrameCount == maximumFramesUntilAddition)
	{
		++currentNumberOfAdditions;
		currentFrameCount = 0;
	}

	if (currentNumberOfAdditions == maximumAdditions + 1)
	{
		currentNumberOfAdditions = 0;
	}

	std::string text = baseText;
	for (int i = 0; i < currentNumberOfAdditions; ++i)
	{
		text += textToPeriodicallyAdd;
	}

	currentText = text;
	++currentFrameCount;
}

std::string PeriodicTextModifier::GetCurrentString() const
{
	return currentText;
}
