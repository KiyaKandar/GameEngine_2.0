#pragma once
#include "../GraphicsModule.h"
#include "../../Meshes/Mesh.h"
#include "../../Meshes/TextMesh.h"
#include "../../Text.h"

struct ProfilerText;
class Database;

class ProfilerModule : public GraphicsModule
{
	public:
		ProfilerModule(const std::string identifier, const Matrix4 projMatrix, const Vector2 resolution,
			Database* database);
		~ProfilerModule();

		void initialise() override;
		void apply() override;

		void linkShaders() override;
		void regenerateShaders() override;

protected:
	void locateUniforms() override;
	Database* database;
	Shader* ProfilerTextShader;

	Matrix4 UIprojMatrix;

	std::vector<ProfilerText>* ProfilerObjects;
	//Perhaps have UIobjects stored in the database, each with their own textMesh?
	TextMesh* textMesh;
	//maybe text has a font object?
	Font* font;

	Vector2 startPosition = Vector2(-600, 300);

private:
	void updateChildPositions();
	void updateGrandchildPositions();

};


