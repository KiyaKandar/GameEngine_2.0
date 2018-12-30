#include "BPLighting.h"

#include "../../GraphicsCommon.h"
#include "../../GraphicsUtility.h"

BPLighting::BPLighting(const std::string identifier, const NCLVector2 resolution,
	Camera* cam, GBufferData* gBuffer, std::vector<Light*>** lights,
	SSAOTextures* ssaoTextures, ShadowData* shadowData)
	: GraphicsModule(identifier, resolution)
{
	camera = cam;
	this->lights = lights;

	this->gBuffer = gBuffer;
	this->ambientTextures = ssaoTextures;

	for (Light* light : **lights)
	{
		lightDatas.push_back(light->GetData());
	}

	this->shadowData = shadowData;

	lightingPassShader = new Shader(SHADERDIR"/SSAO/ssao_lightingvert.glsl", SHADERDIR"/SSAO/ssao_lightingfrag.glsl");
}


void BPLighting::LinkShaders()
{
	lightingPassShader->LinkProgram();
}

void BPLighting::RegenerateShaders()
{
	lightingPassShader->Regenerate();
}

void BPLighting::Initialise()
{
	glGenFramebuffers(1, &FBO);
	LocateUniforms();

	if(lightDatas.size() > 0)
	{
		lightDataBuffer = GraphicsUtility::InitSSBO(1, 1, lightDataBuffer, sizeof(LightData) * lightDatas.size(), &lightDatas, GL_STATIC_COPY);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * lightDatas.size(),
			&lightDatas[0], GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightDataBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

}

void BPLighting::LocateUniforms()
{
	loc_gPosition = glGetUniformLocation(lightingPassShader->GetProgram(), "gPosition");
	loc_gNormal = glGetUniformLocation(lightingPassShader->GetProgram(), "gNormal");
	loc_gAlbedo = glGetUniformLocation(lightingPassShader->GetProgram(), "gAlbedo");
	loc_shadows = glGetUniformLocation(lightingPassShader->GetProgram(), "shadows");
	loc_ambientTextures = glGetUniformLocation(lightingPassShader->GetProgram(), "ssaoTexture");
	loc_texMatrices = glGetUniformLocation(lightingPassShader->GetProgram(), "texMatrices");
	loc_camMatrix = glGetUniformLocation(lightingPassShader->GetProgram(), "camMatrix");

	loc_numShadowCastingLights = glGetUniformLocation(lightingPassShader->GetProgram(), "numShadowCastingLights");
	loc_numberOfLights = glGetUniformLocation(lightingPassShader->GetProgram(), "numberOfLights");
}

void BPLighting::Apply()
{
	lightDatas.clear();
	for (Light* light : **lights)
	{
		lightDatas.push_back(light->GetData());
	}

	if (lightDatas.size() > 0)
	{
		lightDataBuffer = GraphicsUtility::InitSSBO(1, 1, lightDataBuffer, sizeof(LightData) * lightDatas.size(), &lightDatas, GL_STATIC_COPY);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * lightDatas.size(),
			&lightDatas[0], GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightDataBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	LightingPass();
}

void BPLighting::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	SetCurrentShader(lightingPassShader);

	glUniform1i(loc_gPosition, CommonGraphicsData::GPOSITION);
	glUniform1i(loc_gNormal, CommonGraphicsData::GNORMAL);
	glUniform1i(loc_gAlbedo, CommonGraphicsData::GALBEDO);

	//glUniform1f(glGetUniformLocation(lightingPassShader->GetProgram(), "ambientLighting"), ambientLighting);

	glUniform1i(loc_numberOfLights, lightDatas.size());

	glUniform1i(loc_ambientTextures, *ambientTextures->texUnit);

	glUniformMatrix4fv(loc_texMatrices, 1, false, (float*)&shadowData->textureMatrices);

	viewMatrix = camera->BuildViewMatrix();
	glUniformMatrix4fv(loc_camMatrix, 1, false, (float*)&viewMatrix);

	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);

	currentShader->ApplyTexture(CommonGraphicsData::GPOSITION, *gBuffer->gPosition);
	currentShader->ApplyTexture(CommonGraphicsData::GNORMAL, *gBuffer->gNormal);
	currentShader->ApplyTexture(CommonGraphicsData::GALBEDO, *gBuffer->gAlbedo);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "ssaoApplied"), *SSAOApplied);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowsApplied"), *ShadowsApplied);
	glUniform1i(loc_shadows, 5);

	for (int a = 0; a < 1; ++a)
	{
		glActiveTexture(GL_TEXTURE3 + a);
		glBindTexture(GL_TEXTURE_2D, *ambientTextures->textures[a]);
	}

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, shadowData->shadowTex);

	RenderScreenQuad();
	*SSAOApplied = false;
	*ShadowsApplied = false;
}