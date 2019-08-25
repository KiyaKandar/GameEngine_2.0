#pragma once

#include "../GraphicsModule.h"
#include "../../Shaders/ComputeShader.h"
#include "../../Meshes/TextMesh.h"
#include "../../Utilities/GameTimer.h"
#include "../../../Communication/Messages/TextMessage.h"

class Database;

class ScoreCounter : public GraphicsModule
{
public:
	ScoreCounter(const std::string identifier, const NCLVector2 resolution, Database* database);
	~ScoreCounter();

	void bufferScoreHolder(std::string scoreHoldername);

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	GLuint* paintTrailTexture;

	void Clear()
	{
		scoreHolders.clear();
		coloursToCount.clear();

		for (int i = 0; i < 4; ++i)
		{
			scores[i] = 0;
		}

		enabled = false;
	}

private:
	void CalculateScores();
	void DisplayScores();
	void LocateUniforms() override;

	ComputeShader* computeShader;
	Shader* textShader;
	Font* font;
	Database* database;

	GLuint playerScoresSSBO;
	GLuint coloursSSBO;
	GLuint redCounter;
	GLuint yellowCounter;

	int scores[4] = { 0, 0, 0, 0 };
	std::vector<NCLVector4> coloursToCount;
	std::vector<std::string> scoreHolders;

	GameTimer timer;
	float elapsedTime = 0;

	int* scoreData;
};

