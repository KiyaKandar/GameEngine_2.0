#pragma once

#include "Scene Management/SceneManager.h"
#include "Rendering/RenderingSystem.h"
#include "../Input/InputManager.h"
#include "GameplaySystem.h"
#include "../../Audio/AudioSystem.h"
#include "../Launch/Game/GameLoop.h"
#include "Resource Management/Database/TableCreation.h"
#include "Resource Management/Level.h"
#include "../../Input/Recorders/KeyboardMouseRecorder.h"
#include "Profiler/Profiler.h"

class NetworkClient;
class Database;
class ThreadPool;

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

