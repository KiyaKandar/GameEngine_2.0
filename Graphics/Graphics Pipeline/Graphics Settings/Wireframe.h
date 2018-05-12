#pragma once
#include "../GraphicsModule.h"

class Camera;

class Wireframe : public GraphicsModule
{
public:
	Wireframe(const std::string identifier, const NCLVector2 resolution, Camera* camera);
	~Wireframe();

	void linkShaders() override;
	void initialise() override;
	void apply() override;

	void regenerateShaders() override;

	void addLine(NCLVector3 from, NCLVector3 to, NCLVector3 colour);
	void addSphere(NCLVector3 position, float radius, NCLVector3 colour);

private:
	void locateUniforms() override;

	void buildLinesFromSpheres();
	void splitSphere(int circleIndex);
	void renderLines();

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

