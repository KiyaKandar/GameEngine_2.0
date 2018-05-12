#include "BasicGeometry.h"

#include "../../GraphicsCommon.h"
#include "../../Utility/Camera.h"

BasicGeometry::BasicGeometry(const std::string identifier, 
	const NCLMatrix4 projmatrix, const NCLVector2 resolution, Camera* camera, std::vector<SceneNode*>* nodesInFrame)
	: GraphicsModule(identifier, resolution)
{
	this->nodesInFrame = nodesInFrame;
	basicShader = new Shader(SHADERDIR"/basicVertex.glsl", SHADERDIR"/basicFrag.glsl");
	this->camera = camera;

}

BasicGeometry::~BasicGeometry()
{
	delete basicShader;
}

void BasicGeometry::linkShaders()
{
	basicShader->LinkProgram();
}

void BasicGeometry::regenerateShaders()
{
	basicShader->Regenerate();
}

void BasicGeometry::initialise()
{
}

void BasicGeometry::apply()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setCurrentShader(basicShader);
	viewMatrix = camera->buildViewMatrix();

	for (unsigned int i = 0; i < nodesInFrame->size(); ++i)
	{
		updateShaderMatrices();		
		nodesInFrame->at(i)->Draw(*currentShader);
		
	}

}

void BasicGeometry::locateUniforms()
{
}
