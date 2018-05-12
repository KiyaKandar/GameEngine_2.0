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

	void toggleModule(std::string moduleIdentifier);
	void toggleModule(std::string moduleIdentifier, bool enabled);

	void updateModules(float dt);
	void executeModules();

	void addModule(GraphicsModule* module);
	GraphicsModule* getGraphicsModule(std::string moduleIdentifier);

private:
	std::vector<GraphicsModule*> modules;
	GameTimer* parentTimer;
};
