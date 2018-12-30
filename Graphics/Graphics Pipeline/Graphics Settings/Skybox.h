#pragma once

#include "../GraphicsModule.h"
#include "../../Utilities/Maths/Matrix4.h"

#include <vector>
#include <string>

class Skybox : public GraphicsModule
{
public:
	Skybox(const std::string identifier, const NCLVector2 resolution, NCLMatrix4* viewMatrix);
	~Skybox();

	void LinkShaders() override;
	void RegenerateShaders() override;

	void Initialise() override;
	void Apply() override;

	void SetSkyboxTexture(unsigned int newTextureID)
	{
		textureID = newTextureID;
	}

	GLuint* GBufferFBO;
	unsigned int textureID;

private:
	void InitialiseMesh();
	void LocateUniforms() override {}

	NCLMatrix4* viewMatrix;
	Shader* skyboxShader;
	unsigned int VAO, VBO;
};