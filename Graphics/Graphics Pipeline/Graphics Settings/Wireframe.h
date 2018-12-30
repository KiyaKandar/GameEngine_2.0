#pragma once
#include "../GraphicsModule.h"

class Camera;

class Wireframe : public GraphicsModule
{
public:
	Wireframe(const std::string identifier, const NCLVector2 resolution, Camera* camera);
	~Wireframe();

	void LinkShaders() override;
	void Initialise() override;
	void Apply() override;

	void RegenerateShaders() override;

	void AddLine(NCLVector3 from, NCLVector3 to, NCLVector3 colour);
	void AddSphere(NCLVector3 position, float radius, NCLVector3 colour);

private:
	void LocateUniforms() override;

	void BuildLinesFromSpheres();
	void SplitSphere(int circleIndex);
	void RenderLines();

	Shader* debugShader;

	std::vector<NCLVector3> linePoints;
	std::vector<NCLVector3> lineColours;

	std::vector<NCLVector3> spherePositions;
	std::vector<float> radii;
	std::vector<NCLVector3> sphereColours;

	GLuint array;
	GLuint buffers[2];

	Camera* camera;
};

