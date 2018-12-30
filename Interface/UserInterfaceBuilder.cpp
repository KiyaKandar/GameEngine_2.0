#include "UserInterfaceBuilder.h"

#include "../Resource Management/Database/Database.h"
#include "../Graphics/GraphicsCommon.h"

std::vector<Button> UserInterfaceBuilder::BuildButtons(std::string UIFile, Database* database)
{
	XMLParser xmlParser;
	xmlParser.LoadXmlFile(UIFile);

	ButtonActionCreator actionCreator;

	std::vector<Button> buttons;

	for (Node* buttonNode : xmlParser.parsedXml->children)
	{
		buttons.push_back(BuildButton(buttonNode, database, actionCreator));
	}

	return buttons;
}

Button UserInterfaceBuilder::BuildButton(Node* node, Database* database, ButtonActionCreator& actionCreator)
{
	const NCLVector4 colour = GetColour(node->children[0]);;
	const NCLVector2 position = GetTransformInformation(node->children[1]);
	const NCLVector2 scale = GetTransformInformation(node->children[2]);

	const ButtonAction action = actionCreator.CreateButtonAction(node->children[3]);
	const std::string text = node->children[4]->value;
	const std::string meshName = node->children[5]->value;

	Mesh* mesh = static_cast<Mesh*>(
		database->GetTable("UIMeshes")->GetAllResources()->GetResource(meshName));

	Font font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	Button button(colour, position, scale, action, new TextMesh(text, font), mesh);

	if (node->children.size() >= 7)
	{
		for (int i = 6; i < (int)node->children.size(); ++i)
		{
			Button childButton = BuildButton(node->children[i], database, actionCreator);
			button.childButtons.push_back(childButton);
		}
	}

	return button;
}

NCLVector4 UserInterfaceBuilder::GetColour(Node* node)
{
	const float r = std::stof(node->children[0]->value);
	const float g = std::stof(node->children[1]->value);
	const float b = std::stof(node->children[2]->value);
	const float a = std::stof(node->children[3]->value);

	return NCLVector4(r, g, b, a);
}

NCLVector2 UserInterfaceBuilder::GetTransformInformation(Node* node)
{
	const float x = std::stof(node->children[0]->value);
	const float y = std::stof(node->children[1]->value);

	return NCLVector2(x, y);
}
