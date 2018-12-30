#include "SSAO.h"

#include "../../GraphicsUtility.h"
#include "../../GraphicsCommon.h"

const int KERNEL_SIZE = 32;
const int RESOLUTION_SCALE_X = 640;
const int RESOLUTION_SCALE_Y = 360;

SSAO::SSAO(const std::string identifier, const NCLVector2 resolution, Camera* cam, GBufferData* SGBuffer)
	: GraphicsModule(identifier, resolution)
{
	ambientTextures = new SSAOTextures();
	ambientTextures->textures = new GLuint*[1];
	//ambientTextures->texUnit = 0;

	this->SGBuffer = SGBuffer;
	camera = cam;
	this->resolution = resolution;

	//SSAO Shaders
	SSAOCol = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl", SHADERDIR"/SSAO/ssao_frag.glsl");
	SSAOBlur = new Shader(SHADERDIR"/SSAO/ssao_vert.glsl", SHADERDIR"/SSAO/ssao_blurfrag.glsl");

	ambientTextures->textures[CommonGraphicsData::SSAO_INDEX] = &ssaoColorBufferBlur;
	ambientTextures->texUnit = new int(3);

	xSize = 2;// GLConfig::RESOLUTION.x / RESOLUTION_SCALE_X;
	ySize = 2;//GLConfig::RESOLUTION.y / RESOLUTION_SCALE_Y;
}

void SSAO::LinkShaders()
{
	SSAOCol->LinkProgram();
	SSAOBlur->LinkProgram();
}

void SSAO::Initialise()
{
	InitSsaoBuffers();

	//For the SSAO texture
	GenerateSampleKernel();
	GenerateNoiseTexture();

	LocateUniforms();
}

void SSAO::LocateUniforms()
{
	loc_ssaoRadius = glGetUniformLocation(SSAOCol->GetProgram(), "radius");
	loc_ssaoBias = glGetUniformLocation(SSAOCol->GetProgram(), "bias");
	loc_gPosition = glGetUniformLocation(SSAOCol->GetProgram(), "gPosition");
	loc_gNormal = glGetUniformLocation(SSAOCol->GetProgram(), "gNormal");
	loc_texNoise = glGetUniformLocation(SSAOCol->GetProgram(), "texNoise");

	loc_kernel = glGetUniformLocation(SSAOCol->GetProgram(), "samples");

	loc_ssaoInput = glGetUniformLocation(SSAOBlur->GetProgram(), "ssaoInput");
	loc_xSize = glGetUniformLocation(SSAOBlur->GetProgram(), "xSize");
	loc_ySize = glGetUniformLocation(SSAOBlur->GetProgram(), "ySize");
}

void SSAO::Apply()
{
	glDepthMask(GL_FALSE);
	//Generate the SSAO texture
	GenerateSsaoTex();

	//Blur the texture
	SsaoBlurTex();
	glDepthMask(GL_TRUE);

	applied = true;
}

void SSAO::RegenerateShaders()
{
	SSAOCol->Regenerate();
	SSAOBlur->Regenerate();
}

void SSAO::InitSsaoBuffers()
{
	//Create framebuffer to hold SSAO processing stage 
	glGenFramebuffers(1, &ssaoFBO);
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	//SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	GraphicsUtility::CreateScreenTexture(resolution, ssaoColorBuffer, 
		GL_RED, GL_RGB, GL_FLOAT, GL_NEAREST, 0, true);

	GraphicsUtility::VerifyBuffer("SSAO Frame", false);

	//Blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	GraphicsUtility::CreateScreenTexture(resolution, ssaoColorBufferBlur, 
		GL_RED, GL_RGB, GL_FLOAT, GL_NEAREST, 0, true);

	GraphicsUtility::VerifyBuffer("Blur", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::GenerateSampleKernel()
{
	const std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < KERNEL_SIZE; ++i)
	{
		NCLVector3 sample(randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));

		sample.Normalise();
		sample = sample * randomFloats(generator);
		float scale = float(i) / KERNEL_SIZE;

		//Scale samples so they're more aligned to center of kernel
		scale = Lerp(0.1f, 1.0f, scale * scale);//0.1f + (scale * scale) * (1.0f - 0.1f); //Lerp
		sample = sample * scale;
		ssaoKernel.push_back(sample);
	}
}

void SSAO::GenerateNoiseTexture()
{
	const std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	const int xSize = 2;// GLConfig::RESOLUTION.x / 640;
	const int ySize = 2;// 720;// GLConfig::RESOLUTION.y / 360;

	const int noiseSize = (xSize * 2) * (ySize * 2);

	//Generate the texture
	for (unsigned int i = 0; i < noiseSize; i++)
	{
		NCLVector3 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f); // rotate around z-axis (in tangent space)

		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, xSize * 2, ySize * 2, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::GenerateSsaoTex()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SSAOCol);

	glUniform3fv(loc_kernel, KERNEL_SIZE, (float*)&ssaoKernel[0]);

	viewMatrix = camera->BuildViewMatrix();

	//Basic uniforms
	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);

	glUniform1i(loc_ssaoRadius, (GLint)ssaoRadius);
	glUniform1i(loc_ssaoBias, (GLint)ssaoBias);

	//Texture units
	glUniform1i(loc_gPosition, CommonGraphicsData::GPOSITION);
	glUniform1i(loc_gNormal, CommonGraphicsData::GNORMAL);
	glUniform1i(loc_texNoise, NOISE_TEX);

	glUniform1f(glGetUniformLocation(SSAOCol->GetProgram(), "resolutionX"), resolution.x);
	glUniform1f(glGetUniformLocation(SSAOCol->GetProgram(), "resolutionY"), resolution.y);

	currentShader->ApplyTexture(CommonGraphicsData::GPOSITION, *SGBuffer->gPosition);
	currentShader->ApplyTexture(CommonGraphicsData::GNORMAL, *SGBuffer->gNormal);
	currentShader->ApplyTexture(NOISE_TEX, noiseTexture);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::SsaoBlurTex()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(SSAOBlur);

	glUniform1i(loc_xSize, xSize);
	glUniform1i(loc_ySize, ySize);

	glUniform1i(loc_ssaoInput, SSAO_TEX);
	currentShader->ApplyTexture(SSAO_TEX, ssaoColorBuffer);

	RenderScreenQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}