#include "PaintTrail.h"

#include "../../GraphicsCommon.h"
#include "../../GraphicsUtility.h"
#include "../../../Gameplay/GameObject.h"
#include "../Resource Management/Database/Database.h"
#include "../../Rendering/OGLRenderer.h"

#define SHADOWSIZE 4096

PaintTrail::PaintTrail(const std::string identifier, const NCLVector2 resolution, Database* database)
	: GraphicsModule(identifier, resolution)
{
	paintTrailShader = new Shader(SHADERDIR"PaintTrail/paintTrailVert.glsl", SHADERDIR"PaintTrail/paintTrailFrag.glsl");
	this->database = database;
}

PaintTrail::~PaintTrail()
{
	delete paintTrailShader;
}

void PaintTrail::preparePaintSurface(std::vector<GameObject*> surfaceObjects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (GameObject* surfaceObject : surfaceObjects)
	{
		surfaceObject->getSceneNode()->isPaintSurface = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PaintTrail::addPainterObjectForNextFrame(GameObject* painter)
{
	painters.push(painter);
}

void PaintTrail::linkShaders()
{
	paintTrailShader->LinkProgram();
}

void PaintTrail::regenerateShaders()
{
	paintTrailShader->Regenerate();
}

void PaintTrail::initialise()
{
	GraphicsUtility::ClearGLErrorStack();

	glGenFramebuffers(1, &buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);

	glGenTextures(1, &paintTrailTexture);
	GraphicsUtility::CreateScreenTexture(resolution, paintTrailTexture,
		GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, 6, false);
	GraphicsUtility::CheckGLError("PaintTrailTexture");
	GraphicsUtility::VerifyBuffer("paintTrailBuffer", false);

	//Create and attach depth buffer (renderbuffer)
	glDrawBuffers(1, attachment);
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)resolution.x, (GLsizei)resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GraphicsUtility::VerifyBuffer("RBO Depth Paint Trail", false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PaintTrail::apply()
{
	setCurrentShader(paintTrailShader);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);

	viewMatrix = NCLMatrix4::buildViewMatrix(NCLVector3(1, 800, 1), NCLVector3(0, 0, 0));
	textureMatrices = biasMatrix * (CommonGraphicsData::SHARED_PROJECTION_MATRIX * viewMatrix);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);

	glDisable(GL_DEPTH_TEST);
	while (!painters.empty()) 
	{
		GameObject* painter = painters.front();
		painters.pop();

		glUniform4fv(glGetUniformLocation(paintTrailShader->GetProgram(), "baseColour"), 1, (float*)&painter->stats.colourToPaint);
		painter->getSceneNode()->Draw(*paintTrailShader);
	}

	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}