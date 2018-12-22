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

void Startup::renderLoadingScreen()
{
	rendering->renderLoadingScreen(15.0f);
}

void Startup::initialiseSubsystems()
{
	initialiseDatabaseAndTables();
	initialiseAudioSystem();
	animationPlayer = new AnimationManager(database, window->getKeyboard(), camera);
	AnimationPlayer::provide(animationPlayer);
	physics = new PhysicsEngine(database, window->getKeyboard());
	userInterface = new UserInterface(window->getKeyboard(), resolution);
	initialiseLevelSystem();
	initialiseInputSystem();
	PaintGameActionBuilder::initialiseBuilders(database);
	initialiseGameplaySystem();
	addSystemsToEngine();

	game->addWindowToGameLoop(window);
	game->addCameraToGameLoop(camera);
	game->addGameTimerToGameLoop(loopTimer);

	LevelEditor::initialiseLevelEditor(database, gameplay);
}

void Startup::initialiseRenderingSystem()
{
	XMLParser windowConfiguration;
	windowConfiguration.loadXMLFile("../Data/Resources/Graphics Config/windowConfigXML.xml");
	Node* node = windowConfiguration.parsedXml;
	resolution.x = std::stof(node->children[0]->children[0]->value);
	resolution.y = std::stof(node->children[0]->children[1]->value);
	bool fullScreen = node->children[1]->value == "Enabled";

	window = new Window("Game Window", (int)resolution.x, (int)resolution.y, fullScreen);
	window->lockMouseToWindow(true);
	window->showOSPointer(false);

	camera = new Camera(0, 90, NCLVector3(0, 0, 0));

	rendering = new RenderingSystem(window, camera);
	
	nodes = new std::vector<SceneNode*>();
	scene = new SceneManager(camera, nodes);
}

void Startup::startUserInterface()
{
	userInterface->initialise(database);
}

void Startup::startRenderingSystem()
{
	rendering->initialise(database);
	rendering->SetSceneToRender(scene, database);
}

void Startup::setupMeshes()
{
	rendering->setupMeshes();
}

void Startup::initialiseAudioSystem()
{
	audio = new AudioSystem(database, camera);
}

void Startup::initialiseInputSystem()
{
	keyboardAndMouse = new KeyboardMouseRecorder(window->getKeyboard(), window->getMouse());
	playerbase = new PlayerBase(database);
	inputManager = new InputManager(playerbase);
}

void Startup::initialiseDatabaseAndTables()
{
	database = new Database();
	tableCreation = new TableCreation(database);
	profiler = new Profiler(window->getKeyboard(), database, new FPSCounter());
	game->database = database;
}

void Startup::initialiseLevelSystem()
{
	level = new Level(database, scene, physics, userInterface);
}

void Startup::initialiseGameplaySystem()
{
	gameplay = new GameplaySystem(database);
}

void Startup::addSystemsToEngine()
{
	engine->addConcurrentSubsystem(gameplay);
	engine->addConcurrentSubsystem(inputManager);
	engine->addSubsystem(rendering);
	engine->addSubsystem(audio);
	engine->addConcurrentSubsystem(userInterface);
	engine->addConcurrentSubsystem(animationPlayer);
	engine->addConcurrentSubsystem(physics);
	engine->addConcurrentSubsystem(profiler);
	engine->addConcurrentSubsystem(new Console(window->getKeyboard(), camera, window->getMouse()));

	for (Subsystem * subsystem : engine->getSubSystems())
	{
		profiler->addSubsystemTimer(subsystem->getSubsystemName(), subsystem->getTimer());
	}

	engine->RegisterWithProfiler(profiler);
}

void Startup::loadMainMenu()
{
	XMLParser::deleteAllParsedXML();
	level->loadLevelFile(LEVELDIR + "MainMenu.xml", gameplay);
	XMLParser::deleteAllParsedXML();
}

void Startup::loadLevel(std::string levelFile, bool online)
{
	XMLParser::deleteAllParsedXML();
	
	gameplay->setDefaultGameplayScript();
	gameplay->deleteGameObjectScripts();
	physics->InitialiseOctrees(8);
	level->loadLevelFile(LEVELDIR + levelFile, gameplay);

	if (!online)
	{
		playerbase->addNewPlayer(keyboardAndMouse, 0);
		gameplay->connectPlayerbase(inputManager->GetPlayerbase());
	}

	gameplay->compileGameObjectScripts();
	XMLParser::deleteAllParsedXML();
}

void Startup::switchLevel()
{
	XMLParser::deleteAllParsedXML();

	gameplay->setDefaultGameplayScript();
	gameplay->deleteGameObjectScripts();

	rendering->clearScores();
	rendering->clearPainters();

	audio->clearSoundNodesWhenUnloadingLevel();

	animationPlayer->clearAnimations();

	level->unloadLevelWhileKeepingUserInterface();

	XMLParser::deleteAllParsedXML();
}

void Startup::unloadLevel()
{
	XMLParser::deleteAllParsedXML();

	gameplay->setDefaultGameplayScript();
	gameplay->deleteGameObjectScripts();

	rendering->clearScores();
	rendering->clearPainters();

	audio->clearSoundNodesWhenUnloadingLevel();
	level->unloadLevel();

	XMLParser::deleteAllParsedXML();
}

void Startup::beginOnlineLobby()
{
	if (network)
	{
		delete network;
		network = nullptr;
	}

	network = new NetworkClient(keyboardAndMouse, database, inputManager->GetPlayerbase(), gameplay);

	PaintGameActionBuilder::online = true;
	engine->addSubsystem(network);
	network->waitForOtherClients(4);
	network->connectToServer();
	DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime disable"));
}

void Startup::startGameLoop()
{
	game->executeGameLoop();
}



