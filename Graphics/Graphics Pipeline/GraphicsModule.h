#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "GL/glew.h"
#include "GL/wglew.h"
#include "SOIL.h"

#include "../Shaders/Shader.h"
#include "../Utilities/Maths/Matrix4.h"
#include "../Utilities/Maths/Vector2.h"
#include "../GraphicsCommon.h"

struct ShadowData
{
	NCLMatrix4 textureMatrices;
	GLuint shadowTex;
};

struct GBufferData {
	GLuint* gPosition;
	GLuint* gNormal;
	GLuint* gAlbedo;
};

struct SSAOTextures {
	bool* generated;
	GLuint** textures;
	int*	 texUnit;
};

class GraphicsModule
{
public:

	GraphicsModule(const std::string identifier, const NCLVector2 resolution)
	{
		this->resolution = resolution;
		this->identifier = identifier;
		this->enabled = true;
	}

	virtual ~GraphicsModule() {}

	virtual void initialise() = 0;
	virtual void apply() = 0;

	virtual void linkShaders() = 0;
	virtual void regenerateShaders() = 0;

	void update(float dt)
	{
		this->time += dt;
	}

	void setCurrentShader(Shader*s)
	{
		currentShader = s;

		glUseProgram(s->GetProgram());
	}

	void updateShaderMatrices()
	{
		if (currentShader)
		{
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&modelMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, (float*)&textureMatrix);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "camMatrix"), 1, false, (float*)&viewMatrix);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "resolutionY"), (GLint)resolution.y);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "resolutionX"), (GLint)resolution.x);
		}
	}

	const std::string getIdentifier() const
	{
		return identifier;
	}

	const bool isEnabled() const
	{
		return enabled;
	}

	void toggleModule() 
	{
		enabled = !enabled;
	}

	void setIsEnabled(bool newEnabled)
	{
		enabled = newEnabled;
	}

protected:
	virtual void locateUniforms() = 0;

	void renderScreenQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};

			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	Shader* currentShader;
	NCLVector2 resolution;
	NCLMatrix4 modelMatrix;
	NCLMatrix4 viewMatrix;
	NCLMatrix4 textureMatrix;

	std::string identifier;

	bool enabled;
	float time;

};

