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

	void MoveSelectedDown() override;
	void MoveSelectedUp() override;
	void moveSelectedRight();
	void MoveSelectedLeft() override;

	std::vector<Button*>* GetAllButtonsInMenu() override;

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

