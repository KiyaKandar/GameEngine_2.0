#include "GraphicsPipeline.h"

#include "../Utilities/GameTimer.h"

GraphicsPipeline::GraphicsPipeline(GameTimer* parentTimer)
{
	this->parentTimer = parentTimer;
}

GraphicsPipeline::~GraphicsPipeline()
{
	for (GraphicsModule* module : modules)
	{
		delete (module);
	}
	modules.clear();
}

void GraphicsPipeline::toggleModule(std::string moduleIdentifier)
{
	for (GraphicsModule* module : modules)
	{
		if (module->getIdentifier() == moduleIdentifier) 
		{
			module->toggleModule();
			break;
		}
	}
}

void GraphicsPipeline::toggleModule(std::string moduleIdentifier, bool enabled)
{
	for (GraphicsModule* module : modules)
	{
		if (module->getIdentifier() == moduleIdentifier)
		{
			module->setIsEnabled(enabled);
			break;
		}
	}
}

void GraphicsPipeline::updateModules(float dt)
{
	for (GraphicsModule* module : modules)
	{
		module->update(dt);
	}
}

void GraphicsPipeline::executeModules()
{
	for (GraphicsModule* module : modules)
	{
		if (module->isEnabled())
		{
			parentTimer->beginChildTimedSection(module->getIdentifier());
			module->apply();
			parentTimer->endChildTimedSection(module->getIdentifier());
		}
	}
}


void GraphicsPipeline::addModule(GraphicsModule* module)
{
	parentTimer->addChildTimer(module->getIdentifier());
	modules.push_back(module);
}

GraphicsModule* GraphicsPipeline::getGraphicsModule(std::string moduleIdentifier)
{
	for (GraphicsModule* module : modules)
	{
		if (module->getIdentifier() == moduleIdentifier)
		{
			return module;
		}
	}
	return nullptr;
}
