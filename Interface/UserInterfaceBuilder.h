#pragma once

#include "../Graphics/Meshes/Mesh.h"
#include "../Resource Management/XMLParser.h"

#include <vector>
#include "ButtonActionCreator.h"
#include "../Graphics/Meshes/TextMesh.h"

class Database;

struct Button
{
	Button(NCLVector4 colour, NCLVector2 position, NCLVector2 scale,
		ButtonAction action, TextMesh* textMesh, Mesh* mesh)
	{
		this->colour = colour;
		this->position = position;
		this->scale = scale;
		this->action = action;
		this->textMesh = textMesh;
		this->UIMesh = mesh;

		selected = false;
		childrenEnabled = false;
		parent = nullptr;
	}

	std::vector<Button> childButtons;
	Button* parent;
	bool childrenEnabled;

	NCLVector4 colour;
	NCLVector2 position;
	NCLVector2 scale;

	bool selected;

	ButtonAction action;
	TextMesh* textMesh;

	Mesh* UIMesh;
};

class UserInterfaceBuilder
{
public:
	static std::vector<Button> BuildButtons(std::string UIFile, Database* database);

private:
	static Button BuildButton(Node* node, Database* database, ButtonActionCreator& actionCreator);

	static NCLVector4 GetColour(Node* node);
	static NCLVector2 GetTransformInformation(Node* node);
};

