#pragma once

#include "../GraphicsModule.h"
#include "../../Shaders/ComputeShader.h"
#include "../../Meshes/TextMesh.h"
#include "../../Utilities/GameTimer.h"

class Database;

class ScoreCounter : public GraphicsModule
{
public:
	ScoreCounter(const std::string identifier, const NCLVector2 resolution, Database* database);
	~ScoreCounter();

	void bufferScoreHolder(std::string scoreHoldername);

	void linkShaders() override;
	void regenerateShaders() override;

	void initialise() override;
	void apply() override;

	GLuint* paintTrailTexture;

	void clear()
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
	void calculateScores();
	void displayScores();
	void locateUniforms() override;

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

