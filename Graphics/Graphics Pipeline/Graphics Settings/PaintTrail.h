#pragma once

#include "../GraphicsModule.h"

#include <queue>
#include <set>

class Database;
class GameObject;

class PaintTrail : public GraphicsModule
{
public:
	PaintTrail(const std::string identifier, const NCLVector2 resolution, Database* database);
	~PaintTrail();

	void preparePaintSurface(std::vector<GameObject*> surfaceObjects);
	void addPainterObjectForNextFrame(GameObject* painter);

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;
	void RegenerateShaders() override;

	void Clear()
	{
		while (!painters.empty())
		{
			painters.pop();
		}

		existingPainters.clear();
	}

	GLuint paintTrailTexture;
	NCLMatrix4 textureMatrices;

private:
	void LocateUniforms() override;

	Database* database;
	std::queue<GameObject*> painters;
	std::set<GameObject*> existingPainters;

	Shader* paintTrailShader;

	GLuint buffer;
	GLuint rboDepth;
	GLuint attachment[1] = { GL_COLOR_ATTACHMENT6 };
	
	GLint viewMatrixLocation;
	GLint projMatrixLocation;

	NCLMatrix4 localProj;

	bool floorRendered = false;
};

