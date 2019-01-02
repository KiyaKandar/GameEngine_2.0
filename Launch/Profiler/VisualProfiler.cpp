#include "VisualProfiler.h"

#include <iomanip>
#include <sstream>

const NCLVector3 SCREEN_POSITION = NCLVector3(-370.0f, -170.0f, 0.0f);
const NCLVector3 DARK_GREY = NCLVector3(0.75f, 0.75f, 0.75f);
const NCLVector3 LIGHT_GREY = NCLVector3(0.8f, 0.8f, 0.8f);
const NCLVector3 TIMER_BAR_SCALE = NCLVector3(15.0f, 7.5f, 0.1f);
const NCLVector3 TIMER_BAR_COLOUR = NCLVector3(0.3f, 0.3f, 1.0f);
const NCLVector3 TEXT_COLOUR = NCLVector3(1.0f, 1.0f, 1.0f);
const float TEXT_X_AXIS_OFFSET = 240.0f;

const float MARKER_SPACING = 0.5f;
const NCLVector3 MARKER_COLOUR = NCLVector3(0.2f, 1.0f, 0.2f);
const NCLVector3 MARKER_SIZE = NCLVector3(0.5f, 0.0f, 1.0f);

VisualProfiler::VisualProfiler()
{
	GenerateAxisMarkers(MARKER_SPACING);
}

VisualProfiler::~VisualProfiler()
{

}

void VisualProfiler::AddTimerBar(const std::string& name, const GameTimer* timer)
{
	visualTimers.push_back(std::make_pair(name, timer));

	UIQuad addedTimerBar;
	addedTimerBar.colour = TIMER_BAR_COLOUR;

	UIQuad lineBackground;
	switchBackgroundColour ? lineBackground.colour = LIGHT_GREY : lineBackground.colour = DARK_GREY;
	switchBackgroundColour = !switchBackgroundColour;

	lineBackground.scale = NCLVector3(TIMER_BAR_SCALE.x * TIMER_BAR_SCALE.x, TIMER_BAR_SCALE.y, 0.1f);
	lineBackground.screenPosition = NCLVector3(lineBackground.scale.x, 0.0f, 0.0f) + SCREEN_POSITION;

	for (int i = 0; i < numGeneratedMarkers; ++i)
	{
		timerBars[i].scale.y += TIMER_BAR_SCALE.y;
		timerBars[i].screenPosition.y = -timerBars[i].scale.y + SCREEN_POSITION.y + TIMER_BAR_SCALE.y;
	}

	timerBars.push_back(lineBackground);
	timerBars.push_back(addedTimerBar);
}

void VisualProfiler::DisplayVisualTimers()
{
	const float deltaTime = min(updateTimer.GetTimeSinceLastRetrieval(), (1.0f / 60.0f) * 1000.0f);

	if (enabled)
	{
		DisplayTimerBars(deltaTime);
		DisplayTimerBarNames();
		DisplayMarkerLabels();
	}
	else if (clearUIOnNextUpdate)
	{
		if (quadSender.ReadyToSendNextMessage())
		{
			quadSender.SetMessage(UIQuadBatchMessage("RenderingSystem", &emptyUI));
			quadSender.SendTrackedMessage();
			clearUIOnNextUpdate = false;
		}
	}
}

void VisualProfiler::ToggleEnabled()
{
	enabled = !enabled;

	if (!enabled)
	{
		clearUIOnNextUpdate = true;
	}
}

void VisualProfiler::TogglePrecisionMode()
{
	precisionMode = !precisionMode;
}

void VisualProfiler::DisplayTimerBars(const float deltaTime)
{
	if (quadSender.ReadyToSendNextMessage())
	{
		int visualTimerIndex = 0;
		for (int i = numGeneratedMarkers + 1; i < timerBars.size(); i += 2)
		{
			const float timeTaken = visualTimers[visualTimerIndex].second->GetTimeTakenForSection();
			const float timeDifference = (timeTaken * TIMER_BAR_SCALE.x) - timerBars[i].scale.x;

			const float yCoordinate = float(visualTimerIndex) * (-TIMER_BAR_SCALE.y * 2.0f);
			const float xAxisFrameMovement = CalculateTimerBarXAxisScale(deltaTime, timeDifference, timerBars[i].scale.x);

			UpdateBarWidth(timerBars[i], timerBars[i - 1], xAxisFrameMovement, yCoordinate);
			++visualTimerIndex;
		}

		quadSender.SetMessage(UIQuadBatchMessage("RenderingSystem", &timerBars));
		quadSender.SendTrackedMessage();
	}
}

float VisualProfiler::CalculateTimerBarXAxisScale(const float deltaTime, const float timerDifference, const float currentTimerBarValue)
{
	if (precisionMode)
	{
		return CalculateTimerBarXAxisChangeWithPrecision(deltaTime, timerDifference, currentTimerBarValue);
	}
	else
	{
		return CalculateTimerBarXAxisScaleChangeWithInterpolation(deltaTime, timerDifference);
	}
}

float VisualProfiler::CalculateTimerBarXAxisScaleChangeWithInterpolation(const float deltaTime, const float timerDifference) const
{
	const float proportionOfElapsedTime = deltaTime / (0.5f * 1000.0f);
	return timerDifference * proportionOfElapsedTime;
}

float VisualProfiler::CalculateTimerBarXAxisChangeWithPrecision(const float deltaTime, const float timerDifference, const float currentTimerBarValue) const
{
	const float percentageChange = timerDifference / currentTimerBarValue;

	if (percentageChange > 1.0f || percentageChange < 0.0f)
	{
		return timerDifference;
	}
	else if (percentageChange > 0.0f)
	{
		const float proportionOfElapsedTime = deltaTime / (percentageChange * 1000.0f);
		return timerDifference * proportionOfElapsedTime;
	}

	return 0.0f;
}

void VisualProfiler::UpdateBarWidth(UIQuad& timerBar, UIQuad& backgroundBar, 
	const float xAxisWidthChange, const float barPositionYCoordinate)
{
	const float barWidth = timerBar.scale.x + xAxisWidthChange;
	timerBar.scale = NCLVector3(barWidth, TIMER_BAR_SCALE.y, TIMER_BAR_SCALE.z);
	timerBar.screenPosition = NCLVector3(timerBar.scale.x, barPositionYCoordinate, 0.0f) + SCREEN_POSITION;
	timerBar.screenPosition.z = 0.1f;
	backgroundBar.screenPosition.y = timerBar.screenPosition.y;
	backgroundBar.screenPosition.z = 0.9f;
}

void VisualProfiler::DisplayTimerBarNames()
{
	if (timerNameTextSender.ReadyToSendNextMessageGroup())
	{
		std::vector<TextMeshMessage> timerNames;

		int visualTimerIndex = 0;
		for (int i = numGeneratedMarkers + 1; i < timerBars.size(); i += 2)
		{
			const float timerBarLeftHandSidePositionOnXAxis = timerBars[i].screenPosition.x - timerBars[i].scale.x;

			const NCLVector3 textPosition(timerBarLeftHandSidePositionOnXAxis - TEXT_X_AXIS_OFFSET, timerBars[i].screenPosition.y + TIMER_BAR_SCALE.y, 1.0f);

			timerNames.push_back(TextMeshMessage("RenderingSystem", visualTimers[visualTimerIndex].first,
				textPosition, NCLVector3(TIMER_BAR_SCALE.x, TIMER_BAR_SCALE.y * 2.0f, 20.0f), TEXT_COLOUR, true, true));

			++visualTimerIndex;
		}

		timerNameTextSender.SetMessageGroup(timerNames);
		timerNameTextSender.SendMessageGroup();
	}
}

void VisualProfiler::DisplayMarkerLabels()
{
	if (markerLabelTextSender.ReadyToSendNextMessageGroup())
	{
		std::vector<TextMeshMessage> labels;

		for (int i = 0; i < numGeneratedMarkers; i +=2)
		{
			const float millisecondMarker = float(i) * MARKER_SPACING;
			NCLVector3 textPosition = timerBars[i].screenPosition;
			textPosition.y -= timerBars[i].scale.y;

			if (millisecondMarker < 10.0f)
			{
				textPosition -= NCLVector3(4.0f, 0.0f, 0.0f);
			}
			else
			{
				textPosition -= NCLVector3(8.0f, 0.0f, 0.0f);
			}

			labels.push_back(TextMeshMessage("RenderingSystem", to_string(int(millisecondMarker)),
				textPosition, NCLVector3(8.0f, 12.0f, 7.0f), TEXT_COLOUR, true, false));
		}

		markerLabelTextSender.SetMessageGroup(labels);
		markerLabelTextSender.SendMessageGroup();
	}
}

void VisualProfiler::GenerateAxisMarkers(const float millisecondSpacing)
{
	const float singleSpacingUnit = TIMER_BAR_SCALE.x * 2.0f * millisecondSpacing;
	const int numMarkers = ((TIMER_BAR_SCALE.x * TIMER_BAR_SCALE.x * 2.0f) / singleSpacingUnit) + 1;
	
	for (int i = 0; i < numMarkers; ++i)
	{
		const float xCoordinate = singleSpacingUnit * float(i);

		UIQuad marker;
		marker.screenPosition = NCLVector3(SCREEN_POSITION.x + xCoordinate, SCREEN_POSITION.y, 0.0f);
		marker.colour = MARKER_COLOUR;
		marker.scale = MARKER_SIZE;
		timerBars.push_back(marker);
	}

	numGeneratedMarkers = numMarkers;
}
