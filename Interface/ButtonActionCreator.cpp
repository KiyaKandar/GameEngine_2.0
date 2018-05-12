#include "ButtonActionCreator.h"

#include "UserInterfaceBuilder.h"
#include "../Communication/DeliverySystem.h"
#include "../Resource Management/XMLParser.h"

ButtonActionCreator::ButtonActionCreator()
{
	actions.insert({ "None", []()
	{
	} });

	actions.insert({ "Quit", []()
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "Quit"));
	} });

	actions.insert({ "DisableSSAO", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "SSAO", false));
	} });

	actions.insert({ "EnableSSAO", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "SSAO", true));
	} });

	actions.insert({ "EnableSkybox", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "Skybox", true));
	} });

	actions.insert({ "DisableSkybox", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "Skybox", false));
	} });

	actions.insert({ "EnableShadows", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "Shadows", true));
	} });

	actions.insert({ "DisableShadows", []()
	{
		DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "Shadows", false));
	} });
}

ButtonActionCreator::~ButtonActionCreator()
{
}

ButtonAction ButtonActionCreator::createButtonAction(Node* actionNode)
{
	if (actionNode->name == "Start")
	{
		std::string s1 = actionNode->children[0]->value;
		std::string s2 = actionNode->children[1]->value;

		return [s1, s2]()
		{
			DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "Start " + s1 + " " + s2));
		};
	}
	else if (actionNode->name == "Resolution")
	{
		float xResolution = std::stof(actionNode->children[0]->value);
		float yResolution = std::stof(actionNode->children[1]->value);

		return [xResolution, yResolution]()
		{
			DeliverySystem::getPostman()->insertMessage(TextMessage("RenderingSystem", "Resolution " + std::to_string(xResolution) + " " + std::to_string(yResolution)));
		};
	}
	else 
	{
		return actions.at(actionNode->value);
	}
}