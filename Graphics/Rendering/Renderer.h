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

	void renderLoadingScreen(const float& deltatime);

	void initialise(SceneManager* sceneManager, Database* database);
	void update(const float& deltatime);

	void addSceneNode(SceneNode* sceneNode);
	void removeSceneNodeByResourceName(std::string resourcename);

	void toggleModule(const std::string& moduleName, bool enabled);
	GraphicsModule* getGraphicsModule(const std::string& moduleName);

	GraphicsPipeline* getPipeLine() { return &pipeline; }
	SceneManager* getSceneManager()
	{
		return sceneManager;
	}

private:
	void updateScene(const float& msec) override;
	void renderScene() override;

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

