#pragma once

const int SECONDS = 1000;

class FPSCounter
{
public:
	FPSCounter();
	~FPSCounter();

	void CalculateFps(const float& time);

	int frames;
	float frameTime;
	float fps;
};
