#pragma once

#include <functional>
#include <unordered_map>

class Database;
struct Node;

enum PowerUps
{
	SCALE_POWERUP,
	SPEED_POWERUP,
	METEOR_POWERUP

};

typedef std::function<void()> Executable;
typedef std::function<Executable(Node*)> Builder;
typedef std::function<Executable(std::vector<std::string>)> DevConsoleNodeBuilder;

class PaintGameActionBuilder
{
public:
	static void InitialiseBuilders(Database* database);
	static Executable BuildExecutable(Node* node);

	static void UpdateBufferedVariables();

	static std::string localPlayer;
	static bool online;

	static int r1;
	static int others[10];

	static int r1ToSet;
	static int othersToSet[10];
private:
	static std::unordered_map<std::string, Builder> builders;
	static std::string powerUpBuilders[2];
	static Database* database;
};

