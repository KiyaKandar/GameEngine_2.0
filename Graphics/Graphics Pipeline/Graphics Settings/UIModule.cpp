#include "UIModule.h"

#include "../../GraphicsCommon.h"
#include "../Resource Management/Database/Database.h"
#include "UserInterfaceDisplay.h"
#include <iterator>

UIModule::UIModule(const std::string identifier, const NCLVector2 resolution,
	Database* database) : GraphicsModule(identifier, resolution)
{
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	tex = SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	UIShader = new Shader(SHADERDIR"/UIVertex.glsl", SHADERDIR"/UIFrag.glsl");
	UITextShader = new Shader(SHADERDIR"UITextVertex.glsl", SHADERDIR"UITextFrag.glsl");

	this->database = database;
}

UIModule::~UIModule()
{
}

void UIModule::Initialise()
{
}

void UIModule::Apply()
{
	UIObjects = UserInterfaceDisplay::GetInterface()->GetAllButtonsInMenu();

	SetCurrentShader(UIShader);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	viewMatrix.toIdentity();
	textureMatrix.toIdentity();

	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_ORTHOGRAPHIC_MATRIX);

	for (Button* button : *UIObjects)
	{
		glUniform4fv(glGetUniformLocation(UIShader->GetProgram(), "colour"), 1, (float*)&button->colour);

		button->UIMesh->Draw(*currentShader, NCLMatrix4::translation(button->position) * NCLMatrix4::scale(button->scale));

		if (button->childrenEnabled)
		{
			RenderButtons(button->childButtons);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	SetCurrentShader(UITextShader);
	UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_ORTHOGRAPHIC_MATRIX);

	NCLVector3 colour(1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "colour"), 1, (float*)&colour);

	for (Button* button : *UIObjects)
	{
		NCLVector2 buttonSize = button->scale / NCLVector2(4.0f, 1.5f);
		NCLVector2 offset(button->scale.x, 0.0f);
		button->textMesh->Draw(*currentShader, NCLMatrix4::translation(button->position - offset) * NCLMatrix4::scale(NCLVector3(buttonSize.x, buttonSize.y, 1)));

		if (button->childrenEnabled)
		{
			RenderButtonsText(button->childButtons);
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void UIModule::LinkShaders()
{
	UIShader->LinkProgram();
	UITextShader->LinkProgram();
}

void UIModule::RegenerateShaders()
{
	UIShader->Regenerate();
}

void UIModule::LocateUniforms()
{
}

void UIModule::RenderButtons(std::vector<Button>& buttons)
{
	for (Button button : buttons)
	{
		glUniform4fv(glGetUniformLocation(UIShader->GetProgram(), "colour"), 1, (float*)&button.colour);

		button.UIMesh->Draw(*currentShader, NCLMatrix4::translation(button.position) * NCLMatrix4::scale(button.scale));

		if (button.childrenEnabled)
		{
			RenderButtons(button.childButtons);
		}
	}
}

void UIModule::RenderButtonsText(std::vector<Button>& buttons)
{
	for (Button button : buttons)
	{
		NCLVector2 buttonSize = button.scale / NCLVector2(4.0f, 1.5f);
		NCLVector2 offset(button.scale.x, 0.0f);
		button.textMesh->Draw(*currentShader, NCLMatrix4::translation(button.position - offset) * NCLMatrix4::scale(NCLVector3(buttonSize.x, buttonSize.y, 1)));

		if (button.childrenEnabled)
		{
			RenderButtonsText(button.childButtons);
		}
	}
}
