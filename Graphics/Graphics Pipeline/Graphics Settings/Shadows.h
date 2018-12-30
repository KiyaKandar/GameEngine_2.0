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

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;

	void RegenerateShaders() override;

	ShadowData* getShadowData() const
	{
		return shadowData;
	}

	bool applied;

private:
	void LocateUniforms() override
	{}

	//Shadow prep
	void InitShadowTex();
	void InitShadowBuffer();

	//Application
	void DrawShadowScene();

	GLuint shadowFBO;
	ShadowData* shadowData;
	Shader* shadowShader;

	std::vector<Light*>** lights;
	std::vector<SceneNode*>** models;
};

