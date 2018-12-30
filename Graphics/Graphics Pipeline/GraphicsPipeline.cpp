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

void GraphicsPipeline::ToggleModule(std::string moduleIdentifier)
{
	for (GraphicsModule* module : modules)
	{
		if (module->GetIdentifier() == moduleIdentifier) 
		{
			module->ToggleModule();
			break;
		}
	}
}

void GraphicsPipeline::ToggleModule(std::string moduleIdentifier, bool enabled)
{
	for (GraphicsModule* module : modules)
	{
		if (module->GetIdentifier() == moduleIdentifier)
		{
			module->SetIsEnabled(enabled);
			break;
		}
	}
}

void GraphicsPipeline::UpdateModules(float dt)
{
	for (GraphicsModule* module : modules)
	{
		module->Update(dt);
	}
}

void GraphicsPipeline::ExecuteModules()
{
	for (GraphicsModule* module : modules)
	{
		if (module->IsEnabled())
		{
			parentTimer->BeginChildTimedSection(module->GetIdentifier());
			module->Apply();
			parentTimer->EndChildTimedSection(module->GetIdentifier());
		}
	}
}


void GraphicsPipeline::AddModule(GraphicsModule* module)
{
	parentTimer->AddChildTimer(module->GetIdentifier());
	modules.push_back(module);
}

GraphicsModule* GraphicsPipeline::GetGraphicsModule(std::string moduleIdentifier)
{
	for (GraphicsModule* module : modules)
	{
		if (module->GetIdentifier() == moduleIdentifier)
		{
			return module;
		}
	}
	return nullptr;
}
