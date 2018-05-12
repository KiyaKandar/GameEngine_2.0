#include "FPSCounter.h"

FPSCounter::FPSCounter()
{
	frames = 0;
	fps = 0.0f;
	frameTime = 0.0f;
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::calculateFPS(const float& time)
{
	frameTime += time;
	++frames;

	if (frameTime > SECONDS)
	{
		fps = (static_cast<float>(frames) / frameTime) * SECONDS;
		frameTime = 0.0f;
		frames = 0;
	}
}
