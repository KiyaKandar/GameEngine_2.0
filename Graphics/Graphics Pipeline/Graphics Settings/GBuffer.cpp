#include "GBuffer.h"

#include "../Input/Devices/Window.h"
#include "../../GraphicsUtility.h"
#include "../../GraphicsCommon.h"
#include "../../Animation/AnimationPlayer.h"

GBuffer::GBuffer(const std::string identifier, const NCLVector2 resolution, 
	Window* window, Camera* camera, std::vector<SceneNode*>* nodesInFrame)
	: GraphicsModule(identifier, resolution)
{
	time = 0;
	this->nodesInFrame = nodesInFrame;
	this->camera = camera;
	this->window = window;

	geometryPass = new Shader(SHADERDIR"/SSAO/ssao_geometryPerlinVert.glsl",
		SHADERDIR"/SSAO/ssao_geometryfrag.glsl");

	noiseTexture = SOIL_load_OGL_texture(TEXTUREDIR"noiseSampler.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SGBuffer = new GBufferData();
	SGBuffer->gAlbedo = &gAlbedo;
	SGBuffer->gNormal = &gNormal;
	SGBuffer->gPosition = &gPosition;
}

GBuffer::~GBuffer()
{
	delete geometryPass;
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
}

void GBuffer::linkShaders()
{
	geometryPass->LinkProgram();
}

void GBuffer::regenerateShaders()
{
	geometryPass->Regenerate();
}

void GBuffer::initialise()
{
	initGBuffer();
	initAttachments();
	locateUniforms();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::apply()
{
	//Render any geometry to GBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderGeometry(nodesInFrame);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::locateUniforms()
{
	setCurrentShader(geometryPass);

	loc_skybox = glGetUniformLocation(geometryPass->GetProgram(), "skybox");
	loc_cameraPos = glGetUniformLocation(geometryPass->GetProgram(), "cameraPos");
	loc_hasTexture = glGetUniformLocation(geometryPass->GetProgram(), "hasTexture");
	loc_isReflective = glGetUniformLocation(geometryPass->GetProgram(), "isReflective");
	loc_reflectionStrength = glGetUniformLocation(geometryPass->GetProgram(), "reflectionStrength");
	loc_baseColour = glGetUniformLocation(geometryPass->GetProgram(), "baseColour");
	loc_time = glGetUniformLocation(geometryPass->GetProgram(), "time");
	loc_perlin = glGetUniformLocation(geometryPass->GetProgram(), "perlin");
}

void GBuffer::initGBuffer()
{
	GraphicsUtility::ClearGLErrorStack();

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//Position colour buffer
	glGenTextures(1, &gPosition);
	GraphicsUtility::CreateScreenTexture(resolution, gPosition, 
		GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, CommonGraphicsData::GPOSITION, true);
	GraphicsUtility::CheckGLError("GPosition");

	//Normal coluor buffer
	glGenTextures(1, &gNormal);
	GraphicsUtility::CreateScreenTexture(resolution, gNormal, 
		GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, CommonGraphicsData::GNORMAL, false);
	GraphicsUtility::CheckGLError("GNormal");

	//Colour + specular colour buffer
	glGenTextures(1, &gAlbedo);
	GraphicsUtility::CreateScreenTexture(resolution, gAlbedo, 
		GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, CommonGraphicsData::GALBEDO, false);
	GraphicsUtility::CheckGLError("GAlbedo");

	GraphicsUtility::VerifyBuffer("GBuffer", false);
}

void GBuffer::initAttachments()
{
	//Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(3, attachments);

	//Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)resolution.x, (GLsizei)resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GraphicsUtility::VerifyBuffer("RBO Depth GBuffer", false);
}

void GBuffer::renderGeometry(std::vector<SceneNode*>* nodesInFrame)
{
	setCurrentShader(geometryPass);
	viewMatrix = camera->buildViewMatrix();
	updateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(geometryPass->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);
	glUniform2fv(glGetUniformLocation(geometryPass->GetProgram(), "resolution"), 1, (float*)&resolution);

	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "paintTrailTexture"), 6);
	glUniformMatrix4fv(glGetUniformLocation(geometryPass->GetProgram(), "paintTrailTextureMatrix"), 1, false, (float*)paintTextureMatrix);
	currentShader->ApplyTexture(6, *paintTrailTexture);

	//Perlin noise
	glUniform1f(loc_time, time);
	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "perlinTex"), 7);
	currentShader->ApplyTexture(7, noiseTexture);

	glUniform3fv(loc_cameraPos, 1, (float*)&camera->getPosition());
	glActiveTexture(GL_TEXTURE8);
	glUniform1i(loc_skybox, 8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < nodesInFrame->size(); ++i)
	{
		SceneNode* node = nodesInFrame->at(i);
		Mesh* mesh = node->GetMesh();
		UploadSurfaceData(mesh, node);
		UploadAnimationData(mesh);

		node->Draw(*currentShader);
	}
}

void GBuffer::UploadSurfaceData(Mesh* mesh, SceneNode* node)
{
	glUniform1i(loc_perlin, mesh->perlin);
	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "hasTexture"), mesh->hasTexture);
	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "isPaintSurface"), node->isPaintSurface);
	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "isReflective"), node->isReflective);
	glUniform1f(glGetUniformLocation(geometryPass->GetProgram(), "reflectiveStrength"), node->reflectiveStrength);
	glUniform4fv(loc_baseColour, 1, (float*)&node->getColour());
}

void GBuffer::UploadAnimationData(Mesh* mesh)
{
	bool hasAnimations = mesh->scene == nullptr ? false : mesh->scene->HasAnimations();
	glUniform1i(glGetUniformLocation(geometryPass->GetProgram(), "anim"), hasAnimations ? 1 : 0);

	if (hasAnimations)
	{
		std::vector<aiMatrix4x4> transforms;
		AnimationPlayer::getAnimationService()->readAnimationStateForMesh(mesh->getName(), transforms);

		for (int i = 0; i < transforms.size(); i++)
		{
			const string name = "gBones[" + std::to_string(i) + "]";
			glUniformMatrix4fv(glGetUniformLocation(geometryPass->GetProgram(), name.c_str()), 1, GL_TRUE, (const GLfloat*)&transforms[i]);
		}
	}
}

