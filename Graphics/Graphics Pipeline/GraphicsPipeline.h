#pragma once

#include <vector>

#include "GraphicsModule.h"

class GameTimer;

class GraphicsPipeline
{
public:
	GraphicsPipeline() = default;
	GraphicsPipeline(GameTimer* parentTimer);
	~GraphicsPipeline();

	void ToggleModule(std::string moduleIdentifier);
	void ToggleModule(std::string moduleIdentifier, bool enabled);

	void UpdateModules(float dt);
	void ExecuteModules();

	void AddModule(GraphicsModule* module);
	GraphicsModule* GetGraphicsModule(std::string moduleIdentifier);

private:
	std::vector<GraphicsModule*> modules;
	GameTimer* parentTimer;
};
