#include "PaintTrail.h"

#include "../../GraphicsCommon.h"
#include "../../GraphicsUtility.h"
#include "../../../Gameplay/GameObject.h"
#include "../Resource Management/Database/Database.h"
#include "../../Rendering/OGLRenderer.h"

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define SHADOWSIZE 4096

PaintTrail::PaintTrail(const std::string identifier, const NCLVector2 resolution, Database* database)
	: GraphicsModule(identifier, resolution)
{
	viewMatrix = NCLMatrix4::BuildViewMatrix(NCLVector3(1, 800, 1), NCLVector3(0, 0, 0));
	textureMatrices = biasMatrix * (CommonGraphicsData::SHARED_PROJECTION_MATRIX * viewMatrix);
	localProj = CommonGraphicsData::SHARED_PROJECTION_MATRIX;

	paintTrailShader = new Shader(SHADERDIR"PaintTrail/paintTrailVert.glsl", SHADERDIR"PaintTrail/paintTrailFrag.glsl");
	this->database = database;
}

PaintTrail::~PaintTrail()
{
	delete paintTrailShader;
}

void PaintTrail::preparePaintSurface(std::vector<GameObject*> surfaceObjects)
{
	for (GameObject* surfaceObject : surfaceObjects)
	{
		surfaceObject->GetSceneNode()->isPaintSurface = true;
	}
}

void PaintTrail::addPainterObjectForNextFrame(GameObject* painter)
{
	if (std::find(existingPainters.begin(), existingPainters.end(), painter) == existingPainters.end())
	{
		painters.push(painter);
		existingPainters.insert(painter);
	}
}

void PaintTrail::LinkShaders()
{
	paintTrailShader->LinkProgram();
}

void PaintTrail::RegenerateShaders()
{
	paintTrailShader->Regenerate();
}

void PaintTrail::LocateUniforms()
{
	GLuint program = paintTrailShader->GetProgram();
	viewMatrixLocation = glGetUniformLocation(program, "viewMatrix");
	projMatrixLocation = glGetUniformLocation(program, "projMatrix");
}

void PaintTrail::Initialise()
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

	LocateUniforms();
}

void PaintTrail::Apply()
{
	SetCurrentShader(paintTrailShader);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);

	GLuint program = currentShader->GetProgram();
	glUniformMatrix4fv(viewMatrixLocation, 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(projMatrixLocation, 1, false, (float*)&localProj);

	glDisable(GL_DEPTH_TEST);
	
	while (!painters.empty()) 
	{
		GameObject* painter = painters.front();
		painters.pop();

		glUniform4fv(glGetUniformLocation(program, "baseColour"), 1, (float*)&painter->stats.colourToPaint);
		painter->GetSceneNode()->DrawShadow(*paintTrailShader);
	}

	existingPainters.clear();

	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}