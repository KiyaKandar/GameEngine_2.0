#pragma once
#include "../GraphicsModule.h"

struct UIQuad;
class Camera;
class Database;

class Wireframe : public GraphicsModule
{
public:
	Wireframe(const std::string identifier, const NCLVector2 resolution, Camera* camera, Database* database);
	~Wireframe();

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;

	void RegenerateShaders() override;

	void AddLine(NCLVector3 from, NCLVector3 to, NCLVector3 colour);
	void AddSphere(NCLVector3 position, float radius, NCLVector3 colour);
	void AddUIQuads(const std::vector<UIQuad>& uiQuads);

private:
	void LocateUniforms() override;

	void BuildLinesFromSpheres();
	void SplitSphere(int circleIndex);
	void RenderLines();

	void DrawUIQuads();

	Shader* debugShader;
	Shader* quadShader;

	std::vector<NCLVector3> linePoints;
	std::vector<NCLVector3> lineColours;

	std::vector<NCLVector3> spherePositions;
	std::vector<float> radii;
	std::vector<NCLVector3> sphereColours;

	std::vector<UIQuad> quads;

	GLuint array;
	GLuint buffers[2];

	Camera* camera;
	Database* database;
};

