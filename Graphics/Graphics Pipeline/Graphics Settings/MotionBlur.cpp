#include "MotionBlur.h"

#include "../../GraphicsCommon.h"
#include "../../GraphicsUtility.h"
#include "../../GraphicsCommon.h"
#include "../../Utility/Camera.h"

MotionBlur::MotionBlur(const std::string identifier, const NCLMatrix4 projmatrix,
	const NCLVector2 resolution, GBufferData* gBuffer, Camera* camera)
	: GraphicsModule(identifier, resolution)
{
	blurShader = new Shader(SHADERDIR"/MotionBlur/combinevert.glsl", SHADERDIR"/MotionBlur/combinefrag.glsl", "", true);
	this->gBuffer = gBuffer;

	this->camera = camera;
	previousView = camera->buildViewMatrix();
	this->fps = new float(40);
}


MotionBlur::~MotionBlur()
{
	delete fps;
	delete blurShader;
	glDeleteTextures(1, &colourBuffer[0]);
}

void MotionBlur::linkShaders()
{
	blurShader->LinkProgram();
}

void MotionBlur::regenerateShaders()
{
	blurShader->Regenerate();
}

void MotionBlur::initialise()
{
	createTexture();
}

void MotionBlur::createTexture()
{
	GraphicsUtility::ClearGLErrorStack();

	glGenFramebuffers(1, &screenTexFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screenTexFBO);

	glGenTextures(1, colourBuffer);
	GraphicsUtility::CreateScreenTexture(resolution, colourBuffer[0],
		GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, 0, true);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)resolution.x, (GLsizei)resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	glDrawBuffers(1, attachment);

	GraphicsUtility::VerifyBuffer("Motion Blur Buffer", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GraphicsUtility::CheckGLError("Motion blur texture");
}

void MotionBlur::apply()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, screenTexFBO);
	//glEnable(GL_DEPTH_TEST);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	setCurrentShader(blurShader);

	glUniformMatrix4fv(glGetUniformLocation(blurShader->GetProgram(), "projMtx"),
		1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);

	NCLMatrix4 transformEyeSpace = previousView * NCLMatrix4::Inverse(camera->buildViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(blurShader->GetProgram(), "transformEyeSpace"),
		1, false, (float*)&transformEyeSpace);

	/*glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "fps"), static_cast<int>(*fps));*/
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "fps"), 1);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "gPosition"), CommonGraphicsData::GPOSITION);

	currentShader->ApplyTexture(CommonGraphicsData::GPOSITION, *gBuffer->gPosition);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "scene"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, colourBuffer[0]);

	renderScreenQuad();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	previousView = camera->buildViewMatrix();
}