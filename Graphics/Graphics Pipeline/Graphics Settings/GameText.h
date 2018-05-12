#pragma once

#include "../GraphicsModule.h"
#include "../../Meshes/Mesh.h"
#include "../../Meshes/TextMesh.h"
#include "../../Text.h"

class Camera;

class GameText : public GraphicsModule
{
public:
	GameText(const std::string identifier, const NCLVector2 resolution, Camera* camera);
	~GameText();

	void initialise() override;
	void apply() override;

	void linkShaders() override;
	void regenerateShaders() override;

	void bufferText(std::string text, NCLVector3 position, NCLVector3 scale, NCLVector3 colour, bool orthographic, bool hasBackground);

protected:
	void locateUniforms() override;
	Shader* UIShader;
	Shader* UITextShader;
	Shader* textWithBackgrounShader;

	Camera* camera;

	Font* font;
	GLint tex;
	std::vector<std::string> bufferedText;
	std::vector<NCLVector3> bufferedPositions;
	std::vector<NCLVector3> bufferedScales;
	std::vector<NCLVector3> bufferedColours;
	std::vector<bool> bufferedOrthographicUsage;

	std::vector<std::string> bufferedBackgroundText;
	std::vector<NCLVector3> bufferedBackgroundPositions;
	std::vector<NCLVector3> bufferedBackgroundScales;
	std::vector<NCLVector3> bufferedBackgroundColours;
	std::vector<bool> bufferedOBackgroundrthographicUsage;
};

