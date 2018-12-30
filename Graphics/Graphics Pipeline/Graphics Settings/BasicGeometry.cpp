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

void BasicGeometry::LinkShaders()
{
	basicShader->LinkProgram();
}

void BasicGeometry::RegenerateShaders()
{
	basicShader->Regenerate();
}

void BasicGeometry::Initialise()
{
}

void BasicGeometry::Apply()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	SetCurrentShader(basicShader);
	viewMatrix = camera->BuildViewMatrix();

	for (unsigned int i = 0; i < nodesInFrame->size(); ++i)
	{
		UpdateShaderMatrices();		
		nodesInFrame->at(i)->Draw(*currentShader);
		
	}

}

void BasicGeometry::LocateUniforms()
{
}
