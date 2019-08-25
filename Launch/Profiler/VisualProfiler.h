#pragma once

#include "../Communication/Messages/UIQuadBatchMessage.h"
#include "../Utilities/GameTimer.h"

#include <vector>

typedef std::pair<const std::string, const GameTimer*> VisualTimer;

class VisualProfiler
{
public:
	VisualProfiler();
	~VisualProfiler();

	void AddTimerBar(const std::string& name, const GameTimer* timer);
	void DisplayVisualTimers();
	void ToggleEnabled();
	void TogglePrecisionMode();

private:
	void DisplayTimerBars(const float deltaTime);
	float CalculateTimerBarXAxisScale(const float deltaTime, const float timerDifference, const float currentTimerBarValue);
	float CalculateTimerBarXAxisScaleChangeWithInterpolation(const float deltaTime, const float timerDifference) const;
	float CalculateTimerBarXAxisChangeWithPrecision(const float deltaTime, const float timerDifference, const float currentTimerBarValue) const;

	void UpdateBarWidth(UIQuad& timerBar, UIQuad& backgroundBar, 
		const float xAxisWidthChange, const float barPositionYCoordinate);

	void DisplayTimerBarNames();
	void DisplayMarkerLabels();

	void GenerateAxisMarkers(const float millisecondSpacing);

	std::vector<VisualTimer> visualTimers;
	std::vector<UIQuad> timerBars;
	std::vector<UIQuad> emptyUI;
	GameTimer updateTimer;

	bool switchBackgroundColour = false;
	bool enabled = false;
	bool clearUIOnNextUpdate = false;
	bool precisionMode = false;

	int numGeneratedMarkers = 0;
};

