#pragma once

#include "../GraphicsModule.h"

#define SHADOWSIZE 4096

#include "../../Scene Management/SceneNode.h"
#include "../../Utility/Light.h"

#include <vector>

class Shadows : public GraphicsModule
{
public:
	Shadows(const std::string identifier, const NCLVector2 resolution, std::vector<Light*>** lights, std::vector<SceneNode*>** models);

	~Shadows()
	{
		delete shadowShader;
		delete shadowData;

		glDeleteTextures(1, &shadowData->shadowTex);
	}

	void linkShaders() override;
	void initialise() override;
	void apply() override;

	void regenerateShaders() override;

	ShadowData* getShadowData() const
	{
		return shadowData;
	}

	bool applied;

private:
	void locateUniforms() override
	{}

	//Shadow prep
	void initShadowTex();
	void initShadowBuffer();

	//Application
	void drawShadowScene();

	GLuint shadowFBO;
	ShadowData* shadowData;
	Shader* shadowShader;

	std::vector<Light*>** lights;
	std::vector<SceneNode*>** models;
};

