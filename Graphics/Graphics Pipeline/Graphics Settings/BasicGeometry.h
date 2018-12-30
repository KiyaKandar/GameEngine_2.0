#pragma once

#include "../GraphicsModule.h"
#include "../../Meshes/Mesh.h"
#include "../../Scene Management/SceneNode.h"

class Camera;

class BasicGeometry : public GraphicsModule
{
public:
	BasicGeometry(const std::string identifier, const NCLMatrix4 projmatrix,
		const NCLVector2 resolution, Camera* camera, std::vector<SceneNode*>* nodesInFrame);
	~BasicGeometry();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

private:
	std::vector<SceneNode*>* nodesInFrame;


	void LocateUniforms() override;
	Shader* basicShader;

	Camera* camera;
};

