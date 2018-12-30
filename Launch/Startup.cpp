#include "Startup.h"

#include "Networking/NetworkClient.h"
#include "Profiler/FPSCounter.h"
#include "DevConsole\Console.h"
#include "DevConsole/LevelEditor.h"

#include "Resource Management/Database/Database.h"
#include "Resource Management/Level.h"
#include "Resource Management/Database/TableCreation.h"

#include "Rendering/RenderingSystem.h"
#include "../Graphics/Animation/AnimationManager.h"
#include "../Graphics/Animation/AnimationPlayer.h"
#include "../Input/InputManager.h"
#include "GameplaySystem.h"
#include "../../Audio/AudioSystem.h"
#include "../../Input/Recorders/KeyboardMouseRecorder.h"
#include "Profiler/Profiler.h"
#include "../Interface/UserInterface.h"
#include "../Graphics/Rendering/RenderingSystem.h"
#include "../Audio/AudioSystem.h"

#include "../Utilities/FilePaths.h"
#include "../Gameplay/Scripting/PaintGameActionBuilder.h"
#include "Resource Management/XMLWriter.h"

Startup::Startup(ThreadPool* threadpool)
{
	engine = new System(threadpool);
	game = new GameLoop(engine, nullptr, this);
	loopTimer = new GameTimer();
}

Startup::~Startup()
{
	delete engine;
	delete game;
	delete database;
	delete tableCreation;
	delete scene;
	delete level;
	delete loopTimer;
	delete window;
	delete camera;
}

void Startup::RenderLoadingScreen() const
{
	rendering->RenderLoadingScreen(15.0f);
}

void Startup::InitialiseSubsystems()
{
	InitialiseDatabaseAndTables();
	InitialiseAudioSystem();
	animationPlayer = new AnimationManager(database, window->GetKeyboard(), camera);
	AnimationPlayer::Provide(animationPlayer);
	physics = new PhysicsEngine(database, window->GetKeyboard());
	userInterface = new UserInterface(window->GetKeyboard(), resolution);
	InitialiseLevelSystem();
	InitialiseInputSystem();
	PaintGameActionBuilder::InitialiseBuilders(database);
	InitialiseGameplaySystem();
	AddSystemsToEngine();

	game->AddWindowToGameLoop(window);
	game->AddCameraToGameLoop(camera);
	game->AddGameTimerToGameLoop(loopTimer);

	LevelEditor::InitialiseLevelEditor(database, gameplay);
}

void Startup::InitialiseRenderingSystem()
{
	XMLParser windowConfiguration;
	windowConfiguration.LoadXmlFile("../Data/Resources/Graphics Config/windowConfigXML.xml");
	Node* node = windowConfiguration.parsedXml;
	resolution.x = std::stof(node->children[0]->children[0]->value);
	resolution.y = std::stof(node->children[0]->children[1]->value);
	bool fullScreen = node->children[1]->value == "Enabled";

	window = new Window("Game Window", (int)resolution.x, (int)resolution.y, fullScreen);
	window->LockMouseToWindow(true);
	window->ShowOsPointer(false);

	camera = new Camera(0, 90, NCLVector3(0, 0, 0));

	rendering = new RenderingSystem(window, camera);

	nodes = new std::vector<SceneNode*>();
	scene = new SceneManager(camera, nodes);
}

void Startup::StartUserInterface() const
{
	userInterface->Initialise(database);
}

void Startup::StartRenderingSystem() const
{
	rendering->Initialise(database);
	rendering->SetSceneToRender(scene, database);
}

void Startup::SetupMeshes() const
{
	rendering->SetupMeshes();
}

void Startup::InitialiseAudioSystem()
{
	audio = new AudioSystem(database, camera);
}

void Startup::InitialiseInputSystem()
{
	keyboardAndMouse = new KeyboardMouseRecorder(window->GetKeyboard(), window->GetMouse());
	playerbase = new PlayerBase(database);
	inputManager = new InputManager(playerbase);
}

void Startup::InitialiseDatabaseAndTables()
{
	database = new Database();
	tableCreation = new TableCreation(database);
	profiler = new Profiler(window->GetKeyboard(), database, new FPSCounter());
	game->database = database;
}

void Startup::InitialiseLevelSystem()
{
	level = new Level(database, scene, physics, userInterface);
}

void Startup::InitialiseGameplaySystem()
{
	gameplay = new GameplaySystem(database);
}

void Startup::AddSystemsToEngine() const
{
	engine->AddConcurrentSubsystem(gameplay);
	engine->AddConcurrentSubsystem(inputManager);
	engine->AddSubsystem(rendering);
	engine->AddSubsystem(audio);
	engine->AddConcurrentSubsystem(userInterface);
	engine->AddConcurrentSubsystem(animationPlayer);
	engine->AddConcurrentSubsystem(physics);
	engine->AddConcurrentSubsystem(profiler);
	engine->AddConcurrentSubsystem(new Console(window->GetKeyboard(), camera, window->GetMouse()));

	for (Subsystem* subsystem : engine->GetSubSystems())
	{
		profiler->AddSubsystemTimer(subsystem->GetSubsystemName(), subsystem->GetTimer());
	}

	engine->RegisterWithProfiler(profiler);
}

void Startup::LoadMainMenu() const
{
	XMLParser::DeleteAllParsedXml();
	level->LoadLevelFile(LEVELDIR + "MainMenu.xml", gameplay);
	XMLParser::DeleteAllParsedXml();
}

void Startup::LoadLevel(std::string levelFile, bool online) const
{
	XMLParser::DeleteAllParsedXml();

	gameplay->SetDefaultGameplayScript();
	gameplay->DeleteGameObjectScripts();
	physics->InitialiseOctrees(20);
	level->LoadLevelFile(LEVELDIR + levelFile, gameplay);

	if (!online)
	{
		playerbase->AddNewPlayer(keyboardAndMouse, 0);
		gameplay->ConnectPlayerbase(inputManager->GetPlayerbase());
	}

	gameplay->CompileGameObjectScripts();
	XMLParser::DeleteAllParsedXml();
}

void Startup::SwitchLevel() const
{
	XMLParser::DeleteAllParsedXml();

	gameplay->SetDefaultGameplayScript();
	gameplay->DeleteGameObjectScripts();

	rendering->ClearScores();
	rendering->ClearPainters();

	audio->ClearSoundNodesWhenUnloadingLevel();

	animationPlayer->ClearAnimations();

	level->UnloadLevelWhileKeepingUserInterface();

	XMLParser::DeleteAllParsedXml();
}

void Startup::UnloadLevel() const
{
	XMLParser::DeleteAllParsedXml();

	gameplay->SetDefaultGameplayScript();
	gameplay->DeleteGameObjectScripts();

	rendering->ClearScores();
	rendering->ClearPainters();

	audio->ClearSoundNodesWhenUnloadingLevel();
	level->UnloadLevel();

	XMLParser::DeleteAllParsedXml();
}

void Startup::BeginOnlineLobby()
{
	if (network)
	{
		delete network;
		network = nullptr;
	}

	network = new NetworkClient(keyboardAndMouse, database, inputManager->GetPlayerbase(), gameplay);

	PaintGameActionBuilder::online = true;
	engine->AddSubsystem(network);
	network->WaitForOtherClients(4);
	network->ConnectToServer();
	DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime disable"));
}

void Startup::StartGameLoop() const
{
	game->ExecuteGameLoop();
}
