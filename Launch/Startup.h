#pragma once

#include "Scene Management/SceneManager.h"
#include "../Launch/Game/GameLoop.h"
#include "../Utilities/Maths/Vector2.h"

class RenderingSystem;
class AnimationManager;
class InputManager;
class GameplaySystem;
class AudioSystem;
class NetworkClient;
class Database;
class ThreadPool;
class Level;
class TableCreation;
class Keyboard;
class Profiler;

class Startup
{

public:
	Startup(ThreadPool* threadpool);
	~Startup();

	void initialiseRenderingSystem();
	void startUserInterface();
	void startRenderingSystem();
	void setupMeshes();
	void renderLoadingScreen();
	void initialiseSubsystems();
	void startGameLoop();

	void loadMainMenu();
	void loadLevel(std::string levelFile, bool online);
	void switchLevel();
	void unloadLevel();

	void beginOnlineLobby();

private:
	void initialiseAudioSystem();
	void initialiseInputSystem();
	void initialiseDatabaseAndTables();
	void initialiseLevelSystem();
	void initialiseGameplaySystem();

	void addSystemsToEngine();

	System* engine;
	GameLoop* game;

	Database* database;
	TableCreation* tableCreation;
	SceneManager* scene;

	Level* level;

	RenderingSystem* rendering;
	AnimationManager* animationPlayer;
	InputManager* inputManager;
	GameplaySystem* gameplay;
	AudioSystem* audio;
	PhysicsEngine* physics;
	UserInterface* userInterface;
	NetworkClient* network = nullptr;
	Profiler* profiler;
	Keyboard* keyboard;

	GameTimer* loopTimer;
	Window* window;

	Camera* camera;

	InputRecorder* keyboardAndMouse;
	PlayerBase* playerbase;

	SceneNode* node;
	std::vector<SceneNode*>* nodes;
	NCLVector2 resolution;
};

