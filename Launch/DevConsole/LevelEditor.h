#pragma once

#include <functional>
#include <string>
#include <map>
#include <vector>

class Database;
class GameplaySystem;

typedef std::function<void(std::vector<std::string>)> LevelEditorAction;

class LevelEditor
{
public:
	static void initialiseLevelEditor(Database* database, GameplaySystem* gameplay);
	static void executeDevConsoleLine(std::string devConsoleLine);

private:
	static Database* database;
	static GameplaySystem* gameplay;
	static std::string levelFile;
	static std::map<std::string, LevelEditorAction> actions;
};

