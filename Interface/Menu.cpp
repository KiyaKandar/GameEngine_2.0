#include "Menu.h"

Menu::Menu(std::string buttonFile, Database* database)
{
	menu = UserInterfaceBuilder::buildButtons(buttonFile, database);

	for (int i = 0; i < (int)menu.size(); ++i)
	{
		allButtons.push_back(&menu[i]);
	}

	for (Button& button : menu)
	{
		buildTree(&button);
	}

	selectedRowIndex = 0;
	depth = 0;
	indexes.push_back(selectedRowIndex);
	HighlightSelectedButton();
}

Menu::~Menu()
{
}

void Menu::HighlightSelectedButton()
{
	std::vector<Button>* column = &menu;

	for (int i = 0; i < depth; ++i)
	{
		column->at(indexes[i + 1]).childrenEnabled = true;
		column = &(column->at(indexes[i + 1]).childButtons);
	}

	currentColumnSize = column->size();

	parentOfLastButtonPressed = column->at(selectedRowIndex).parent;
	currentSelectedButton = &column->at(selectedRowIndex);

	slectedButtonDefaultColour = column->at(selectedRowIndex).colour;
	column->at(selectedRowIndex).colour = NCLVector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Menu::UnhighlightButton()
{
	std::vector<Button>* column = &menu;

	for (int i = 0; i < depth; ++i)
	{
		column = &(column->at(indexes[i + 1]).childButtons);
	}

	column->at(selectedRowIndex).colour = slectedButtonDefaultColour;
}

void Menu::ExecuteSelectedButton()
{
	if (currentSelectedButton->childButtons.size() > 0)
	{
		moveSelectedRight();
	}
	else
	{
		std::vector<Button>* column = &menu;

		for (int i = 0; i < depth; ++i)
		{
			column = &(column->at(indexes[i + 1]).childButtons);
		}

		if (column->at(selectedRowIndex).childButtons.size() == 0)
		{
			column->at(selectedRowIndex).action();
		}
	}
}

void Menu::moveSelectedDown()
{
	if (selectedRowIndex < currentColumnSize - 1)
	{
		UnhighlightButton();
		++selectedRowIndex;
		HighlightSelectedButton();
	}
}

void Menu::moveSelectedUp()
{
	if (selectedRowIndex > 0)
	{
		UnhighlightButton();
		--selectedRowIndex;
		HighlightSelectedButton();
	}
}

void Menu::moveSelectedRight()
{
	UnhighlightButton();
	++depth;
	indexes.push_back(selectedRowIndex);
	selectedRowIndex = 0;
	HighlightSelectedButton();
}

void Menu::moveSelectedLeft()
{
	if (currentSelectedButton->parent != nullptr)
	{
		parentOfLastButtonPressed->childrenEnabled = false;

		UnhighlightButton();
		--depth;
		indexes.pop_back();
		selectedRowIndex = 0;
		HighlightSelectedButton();
	}
}

std::vector<Button*>* Menu::getAllButtonsInMenu()
{
	return &allButtons;
}

void Menu::buildTree(Button* button)
{
	for (Button& child : button->childButtons)
	{
		child.parent = button;

		buildTree(&child);
	}
}
