#pragma once

#include "../GraphicsModule.h"
#include "../../Utility/Camera.h"
#include "../../Utility/Light.h"

#include <vector>

class BPLighting : public GraphicsModule
{
public:
	BPLighting(const std::string identifier, const NCLVector2 resolution, 
		Camera* cam, GBufferData* gBuffer, std::vector<Light*>** lights,
		SSAOTextures* ssaoTextures, ShadowData* shadowData);

	virtual ~BPLighting()
	{
		delete lightingPassShader;
	}

	void LinkShaders()  override;
	void RegenerateShaders()  override;

	void UpdateShadowData(ShadowData* shadowData) 
	{
		this->shadowData = shadowData;
	}

	void Initialise() override;
	void Apply() override;

	GLuint FBO;
	float ambientLighting;
	bool* SSAOApplied;
	bool* ShadowsApplied;

private:
	void LocateUniforms() override;
	void LightingPass();

	//Uniform locations
	GLint lightDataBuffer;

	GLint loc_gPosition;
	GLint loc_gNormal;
	GLint loc_gAlbedo;
	GLint loc_shadows;
	GLint loc_ambientTextures;
	GLint loc_texMatrices;
	GLint loc_numberOfLights;
	GLint loc_camMatrix;
	GLint loc_cameraPos;
	GLint loc_numShadowCastingLights;

	Shader*		lightingPassShader;
	Camera*		camera;
	std::vector<LightData> lightDatas;
	std::vector<Light*>** lights;
	ShadowData* shadowData;
	GBufferData*	gBuffer;
	SSAOTextures* ambientTextures;
};

