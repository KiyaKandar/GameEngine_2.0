#pragma once

#include "OGLRenderer.h"
#include "../Graphics Pipeline/GraphicsPipeline.h"
#include "../Graphics Pipeline/PipelineConfiguration.h"
#include "../Scene Management/SceneManager.h"
#include "../../Launch/Systems/Subsystem.h"

class Window;
class Camera;
class NCLMatrix4;
class Database;
class GameTimer;

class Renderer : public OGLRenderer
{
public:
	Renderer();
	Renderer(GameTimer* parentTimer, Window* window, Camera* camera);
	~Renderer();

	void RenderLoadingScreen(const float& deltatime);

	void Initialise(SceneManager* sceneManager, Database* database);
	void Update(const float& deltatime);

	void AddSceneNode(SceneNode* sceneNode);
	void RemoveSceneNodeByResourceName(std::string resourcename);

	void ToggleModule(const std::string& moduleName, bool enabled);
	GraphicsModule* GetGraphicsModule(const std::string& moduleName);

	GraphicsPipeline* GetPipeLine() { return &pipeline; }
	SceneManager* GetSceneManager()
	{
		return sceneManager;
	}

private:
	void UpdateScene(const float& msec) override;
	void RenderScene() override;

	GraphicsPipeline pipeline;
	PipelineConfiguration graphicsConfig;

	SceneManager* sceneManager; 
	Window* window; 
	Camera* camera; 
	GameTimer* parentTimer;

	NCLVector2 resolution;
	NCLMatrix4 globalProjectionMatrix;
	NCLMatrix4 globalOrthographicMatrix;

	SceneNode* loadingScreenMesh;
	Shader* loadingScreenShader;
};

