#pragma once

#include "../GraphicsModule.h"
#include "../../Meshes/Mesh.h"
#include "../Interface/UserInterfaceBuilder.h"
#include "../../Meshes/TextMesh.h"
#include "../../Text.h"

class UIModule : public GraphicsModule
{
public:
	UIModule(const std::string identifier, const NCLVector2 resolution,
		Database* database);
	~UIModule();

	void initialise() override;
	void apply() override;

	void linkShaders() override;
	void regenerateShaders() override;

protected:
	void locateUniforms() override;
	Database* database;
	Shader* UIShader;
	Shader* UITextShader;

	NCLMatrix4 UIprojMatrix;
	std::vector<Button*>* UIObjects;

	void renderButtons(std::vector<Button>& buttons);
	void renderButtonsText(std::vector<Button>& buttons);
	//Perhaps have UIobjects stored in the database, each with their own textMesh?
	TextMesh* textMesh;
	//maybe text has a font object?
	Font* font;
	GLint tex;
};

