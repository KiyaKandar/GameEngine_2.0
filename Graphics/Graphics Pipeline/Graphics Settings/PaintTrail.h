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

	void linkShaders() override;
	void initialise() override;
	void apply() override;
	void regenerateShaders() override;

	void clear()
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
	void locateUniforms() override;

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

