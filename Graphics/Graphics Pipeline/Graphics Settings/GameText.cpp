#include "GameText.h"

#include "../../GraphicsCommon.h"
#include "../Graphics/Utility/Camera.h"

GameText::GameText(const std::string identifier, const NCLVector2 resolution, Camera* camera)
	: GraphicsModule(identifier, resolution)
{
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	tex = SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	UIShader = new Shader(SHADERDIR"/UIVertex.glsl", SHADERDIR"/UIFrag.glsl");
	UITextShader = new Shader(SHADERDIR"UITextVertex.glsl", SHADERDIR"UITextFrag.glsl");
	textWithBackgrounShader = new Shader(SHADERDIR"UITextVertex.glsl", SHADERDIR"/ScoreText/textFrag.glsl");

	this->camera = camera;
}


GameText::~GameText()
{
	delete UIShader;
	delete UITextShader;
	delete textWithBackgrounShader;
}

void GameText::Initialise()
{
}

void GameText::Apply()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//viewMatrix.toIdentity();
	textureMatrix.ToIdentity();


	SetCurrentShader(UITextShader);
	UpdateShaderMatrices();

	for (int i = 0; i < (int)bufferedText.size(); ++i)
	{
		if (bufferedOrthographicUsage[i] == true)
		{
			viewMatrix.ToIdentity();
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_ORTHOGRAPHIC_MATRIX);
		}
		else
		{
			viewMatrix = camera->BuildViewMatrix();
			bufferedScales[i].x = -bufferedScales[i].x;
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);
		}

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "colour"), 1, (float*)&bufferedColours[i]);

		TextMesh textMesh(bufferedText[i], *font);
		textMesh.Draw(*currentShader, NCLMatrix4::Translation(bufferedPositions[i]) * NCLMatrix4::Scale(NCLVector3(bufferedScales[i].x, bufferedScales[i].y, 1)));
	}

	bufferedText.clear();
	bufferedPositions.clear();
	bufferedScales.clear();
	bufferedOrthographicUsage.clear();
	bufferedColours.clear();

	if (!bufferedBackgroundText.empty())
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textureMatrix.ToIdentity();

		SetCurrentShader(textWithBackgrounShader);
		UpdateShaderMatrices();

		for (int i = 0; i < (int)bufferedBackgroundText.size(); ++i)
		{
			viewMatrix.ToIdentity();
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_ORTHOGRAPHIC_MATRIX);

			NCLVector3 colour(bufferedBackgroundColours[i].x, bufferedBackgroundColours[i].y, bufferedBackgroundColours[i].z);
			glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "colour"), 1, (float*)&colour);

			TextMesh textMesh(bufferedBackgroundText[i], *font);
			textMesh.Draw(*currentShader, NCLMatrix4::Translation(bufferedBackgroundPositions[i]) * NCLMatrix4::Scale(bufferedBackgroundScales[i]));
		}

		bufferedBackgroundText.clear();
		bufferedBackgroundPositions.clear();
		bufferedBackgroundScales.clear();
		bufferedOBackgroundrthographicUsage.clear();
		bufferedBackgroundColours.clear();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void GameText::LinkShaders()
{
	UIShader->LinkProgram();
	UITextShader->LinkProgram();
	textWithBackgrounShader->LinkProgram();
}

void GameText::RegenerateShaders()
{
	UIShader->Regenerate();
}

void GameText::BufferText(std::string text, NCLVector3 position, NCLVector3 scale, NCLVector3 colour, bool orthographic, bool hasBackground)
{
	if (hasBackground)
	{
		bufferedBackgroundText.push_back(text);
		bufferedBackgroundPositions.push_back(position);
		bufferedBackgroundScales.push_back(scale);
		bufferedBackgroundColours.push_back(colour);
		bufferedOBackgroundrthographicUsage.push_back(orthographic);
	}
	else
	{
		bufferedText.push_back(text);
		bufferedPositions.push_back(position);
		bufferedScales.push_back(scale);
		bufferedColours.push_back(colour);
		bufferedOrthographicUsage.push_back(orthographic);
	}
}

void GameText::LocateUniforms()
{
}
