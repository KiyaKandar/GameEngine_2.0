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
class UserInterface;

class Startup
{
public:
	Startup(ThreadPool* threadpool);
	~Startup();

	void InitialiseRenderingSystem();
	void StartUserInterface() const;
	void StartRenderingSystem() const;
	void SetupMeshes() const;
	void RenderLoadingScreen() const;
	void InitialiseSubsystems();
	void StartGameLoop() const;

	void LoadMainMenu() const;
	void LoadLevel(std::string levelFile, bool online) const;
	void SwitchLevel() const;
	void UnloadLevel() const;

	void BeginOnlineLobby();

private:
	void InitialiseAudioSystem();
	void InitialiseInputSystem();
	void InitialiseDatabaseAndTables();
	void InitialiseLevelSystem();
	void InitialiseGameplaySystem();

	void AddSystemsToEngine() const;

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
