#include "Shadows.h"

#include "../../GraphicsCommon.h"
#include "../../GraphicsUtility.h"
#include "../../Rendering/OGLRenderer.h"

Shadows::Shadows(const std::string identifier, const NCLVector2 resolution, 
	std::vector<Light*>** lights, std::vector<SceneNode*>** models)
	: GraphicsModule(identifier, resolution)
{
	shadowData = new ShadowData();

	shadowShader = new Shader(SHADERDIR"shadowvert.glsl", SHADERDIR"shadowfrag.glsl");

	this->resolution = resolution;
	this->lights = lights;
	this->models = models;
}

void Shadows::LinkShaders()
{
	shadowShader->LinkProgram();
}

void Shadows::RegenerateShaders()
{
	shadowShader->Regenerate();
}

void Shadows::Initialise()
{
	//Shadow prep
	InitShadowTex();
	InitShadowBuffer();
}

void Shadows::Apply()
{
	DrawShadowScene();
	applied = true;
}

void Shadows::InitShadowTex()
{
	glGenTextures(1, &shadowData->shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowData->shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	GraphicsUtility::CheckGLError("Shadow textures");
}

void Shadows::InitShadowBuffer()
{
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowData->shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GraphicsUtility::VerifyBuffer("Shadow Buffer", false);
}

void Shadows::DrawShadowScene()
{
	glCullFace(GL_FRONT);
	SetCurrentShader(shadowShader);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);

	for (auto i = (**lights).begin(); i != (**lights).end(); ++i)
	{
		if ((*i)->IsShadowCasting())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			viewMatrix = NCLMatrix4::buildViewMatrix((*i)->GetPosition(), NCLVector3(0, 0, 0));
			shadowData->textureMatrices = biasMatrix * (CommonGraphicsData::SHARED_PROJECTION_MATRIX * viewMatrix);

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);

			for (SceneNode* m : **models)
			{
				if (m->isEnabled)
				{
					UploadAnimationData(m, currentShader);
					m->DrawShadow(*shadowShader);
				}
				
			}
		}
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, (GLsizei)resolution.x, (GLsizei)resolution.y);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}