#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "../Utilities/Maths/Vector4.h"
#include "Renderer.h"
#include <memory>
#include "../Resource Management/XMLParser.h"
#include "../Graphics Pipeline/GraphicsPipeline.h"

class Database;

class RenderingSystem : public Subsystem
{
public:
	RenderingSystem(Window* window, Camera* camera);
	~RenderingSystem();

	void initialise(Database* database);
	void renderLoadingScreen(const float& deltaTime);

	void setupMeshes();
	void SetSceneToRender(SceneManager* scene, Database* database);
	void updateNextFrame(const float& deltaTime) override;

	void clearScores()
	{
		static_cast<ScoreCounter*>(renderer->getPipeLine()->getGraphicsModule("ScoreCounter"))->clear();
	}

	void clearPainters()
	{
		static_cast<PaintTrail*>(renderer->getPipeLine()->getGraphicsModule("PaintTrail"))->clear();
	}

private:
	std::unique_ptr<Renderer> renderer;
	std::map<std::string, bool> graphicsConfig;
	GraphicsPipeline pipeline;
	SceneManager* scene;
	Camera* camera;
	bool blockCamera = false;
};