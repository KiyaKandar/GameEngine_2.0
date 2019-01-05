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

	void Initialise(Database* database);
	void RenderLoadingScreen(const float& deltaTime);

	void InitialiseGraphicalAssets();
	void SetSceneToRender(SceneManager* scene, Database* database);
	void UpdateNextFrame(const float& deltaTime) override;

	void ClearScores()
	{
		static_cast<ScoreCounter*>(renderer->GetPipeLine()->GetGraphicsModule("ScoreCounter"))->Clear();
	}

	void ClearPainters()
	{
		static_cast<PaintTrail*>(renderer->GetPipeLine()->GetGraphicsModule("PaintTrail"))->Clear();
	}

private:
	std::unique_ptr<Renderer> renderer;
	std::map<std::string, bool> graphicsConfig;
	GraphicsPipeline pipeline;
	SceneManager* scene;
	Camera* camera;
	bool blockCamera = false;
};