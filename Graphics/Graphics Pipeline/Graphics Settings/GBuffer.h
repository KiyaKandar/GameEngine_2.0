#pragma once

#include "../GraphicsModule.h"

#include "../../Utility/Camera.h"
#include "../../Meshes/Mesh.h"
#include "../../Meshes/SubMesh.h"

#include <vector>
#include "Skybox.h"

#include "../../Scene Management/SceneManager.h"

class Window;

class GBuffer : public GraphicsModule
{
public:
	GBuffer(const std::string identifier, const NCLVector2 resolution, 
		Window* window, Camera* camera, std::vector<SceneNode*>* nodesInFrame);
	virtual ~GBuffer();

	void linkShaders() override;
	void regenerateShaders() override;

	void initialise() override;
	void apply() override;

	GBufferData* getGBuffer() const
	{
		return SGBuffer;
	}

	void setReflectionTextureID(unsigned int newTextureID)
	{
		textureID = newTextureID;
	}

	GLuint rboDepth;
	GLuint gBuffer;

	Skybox* skybox;
	NCLMatrix4* paintTextureMatrix;
	GLuint* paintTrailTexture;

private:
	void locateUniforms() override;
	void initGBuffer();
	void initAttachments();

	void renderGeometry(std::vector<SceneNode*>* nodesInFrame);

	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo;
	GLuint attachments[3] =
	{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	Shader* geometryPass;

	Camera* camera;
	Window* window;

	std::vector<SceneNode*>* nodesInFrame;

	GBufferData* SGBuffer;

	unsigned int textureID;

	GLint loc_skybox;
	GLint loc_cameraPos;
	GLint loc_hasTexture;
	GLint loc_isReflective;
	GLint loc_reflectionStrength;
	GLint loc_baseColour;
	GLint loc_time;
	GLint loc_perlin;

	GLint noiseTexture;
};

