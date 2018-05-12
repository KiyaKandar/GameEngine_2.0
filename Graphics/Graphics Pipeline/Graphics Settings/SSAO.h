#pragma once

#include "../GraphicsModule.h"

#include "../../Utility/Camera.h"

#include <vector>
#include <string>
#include <random>

const int NOISE_TEX = 2;
const int SSAO_TEX = 0;

class SSAO : public GraphicsModule
{
public:
	SSAO(const std::string identifier, const NCLVector2 resolution, Camera* cam, GBufferData* SGBuffer);

	virtual ~SSAO()
	{
		delete SSAOCol;
		delete SSAOBlur;
		delete SGBuffer;
		delete ambientTextures;

		glDeleteTextures(1, &ssaoColorBuffer);
		glDeleteTextures(1, &ssaoColorBufferBlur);
	}

	void linkShaders() override;
	void initialise() override;
	void apply() override;

	void regenerateShaders() override;

	void setViewMatrix(NCLMatrix4 mat)
	{
		viewMatrix = mat;
	}

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	SSAOTextures* getSSAOTextures()
	{
		return ambientTextures;
	}

	bool applied;

private:
	void locateUniforms() override;

	Camera* camera;
	SSAOTextures* ambientTextures;
	GBufferData* SGBuffer;

	//Init Functions
	void initSSAOBuffers();
	void generateSampleKernel();
	void generateNoiseTexture();

	/*
	  Render Functions.
	  MUST TAKE PLACE IN THIS ORDER.
	*/
	void generateSSAOTex();
	void SSAOBlurTex();

	//SSAO Vars
	GLuint ssaoFBO;
	GLuint ssaoBlurFBO;
	GLuint noiseTexture;
	GLuint ssaoColorBuffer, ssaoColorBufferBlur;

	//Uniform locations
	GLint loc_ssaoRadius;
	GLint loc_ssaoBias;
	GLint loc_gPosition;
	GLint loc_gNormal;
	GLint loc_texNoise;
	GLint loc_ssaoInput;
	GLint loc_xSize;
	GLint loc_ySize;

	GLuint loc_kernel;

	std::vector<NCLVector3> ssaoKernel;
	std::vector<NCLVector3> ssaoNoise;

	//SSAO Uniforms
	float ssaoRadius = 0.4f;
	float ssaoBias = 0.020f;

	//SSAO Shaders
	Shader* SSAOCol;
	Shader* SSAOBlur;

	int xSize;
	int ySize;
};

