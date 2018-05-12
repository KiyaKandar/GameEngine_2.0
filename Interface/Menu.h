#pragma once

#include "UserInterfaceBuilder.h"
#include "UserInterfaceService.h"

#include <string>
#include <vector>

class Database;

class Menu : public UserInterfaceService
{
public:
	explicit Menu(std::string buttonFile, Database* database);
	~Menu();

	void HighlightSelectedButton();
	void UnhighlightButton();
	void ExecuteSelectedButton() override;

	void moveSelectedDown() override;
	void moveSelectedUp() override;
	void moveSelectedRight();
	void moveSelectedLeft() override;

	std::vector<Button*>* getAllButtonsInMenu() override;

private:
	void buildTree(Button* button);

	std::vector<Button> menu;
	std::vector<Button*> allButtons;

	NCLVector4 slectedButtonDefaultColour;
	int selectedRowIndex;
	int depth;
	std::vector<int> indexes;

	Button* parentOfLastButtonPressed;
	Button* currentSelectedButton;

	int currentColumnSize = 0;
};

