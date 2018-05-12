#pragma once

#include "../Systems/System.h"
#include "../../Utilities/GameTimer.h"
#include "../../Input/Devices/Window.h"




#include <memory>
#include "Scene Management/SceneManager.h"
#include "Rendering/RenderingSystem.h"
#include "../Input/InputManager.h"
#include "GameplaySystem.h"
#include "../../Interface/UserInterface.h"

#include "../../Audio/AudioSystem.h"
#include "../../Physics/PhysicsEngine.h"
#include "../Communication/MessageProcessor.h"

class Startup;

class GameLoop
{
public:
	GameLoop(System* gameSystem, Database* database, Startup* startup);
	~GameLoop();

	void executeGameLoop();
	void updateGameObjects(float deltaTime);

	void addCameraToGameLoop(Camera* camera)
	{
		this->camera = camera;
	}

	void addWindowToGameLoop(Window* window)
	{
		this->window = window;
	}

	void addGameTimerToGameLoop(GameTimer* timer)
	{
		loopTimer = timer;
	}

	Database* database;
private:
	MessageProcessor incomingMessages;
	System* engine;
	GameTimer* loopTimer;
	Window* window;
	Camera* camera;

	bool loadingLevel = false;
	bool quit = false;
	float deltaTimeMultiplier = 1.0f;
};

