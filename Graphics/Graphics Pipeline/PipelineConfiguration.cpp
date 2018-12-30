#include "PipelineConfiguration.h"

#include "Graphics Settings/BasicGeometry.h"
#include "../Resource Management/Database/Database.h"

PipelineConfiguration::PipelineConfiguration()
{
	this->sceneManager = nullptr;
	this->window = nullptr;
	this->resolution = NCLVector2();
	this->camera = nullptr;
}

PipelineConfiguration::PipelineConfiguration(SceneManager* sceneManager, Window* window, 
	Camera* camera, NCLVector2 resolution)
{
	this->sceneManager = sceneManager;
	this->window = window;
	this->resolution = resolution;
	this->camera = camera;
}

PipelineConfiguration::~PipelineConfiguration()
{
}

void PipelineConfiguration::InitialiseModules(Database* database)
{
	paintTrail = new PaintTrail("PaintTrail", resolution, database);
	paintTrail->LinkShaders();
	paintTrail->Initialise();

	gBuffer = new GBuffer("gbuffer", resolution, window, camera, sceneManager->GetSceneNodesInFrustum());
	gBuffer->LinkShaders();
	gBuffer->Initialise();
	gBuffer->paintTextureMatrix = &paintTrail->textureMatrices;
	gBuffer->paintTrailTexture = &paintTrail->paintTrailTexture;

	skybox = new Skybox("Skybox", resolution, &camera->viewMatrix);
	skybox->LinkShaders();
	skybox->Initialise();
	skybox->GBufferFBO = &gBuffer->gBuffer;

	ssao = new SSAO("SSAO", resolution, camera, gBuffer->GetGBuffer());
	ssao->LinkShaders();
	ssao->Initialise();

	shadowTex = new Shadows("Shadows", resolution, sceneManager->GetAllLights(), sceneManager->GetAllNodes());
	shadowTex->LinkShaders();
	shadowTex->Initialise();

	bpLighting = new BPLighting("BPLighting", resolution, camera, gBuffer->GetGBuffer(), 
		sceneManager->GetAllLights(), ssao->GetSSAOTextures(), shadowTex->getShadowData());
	bpLighting->LinkShaders();
	bpLighting->Initialise();
	bpLighting->SSAOApplied = &ssao->applied;
	bpLighting->ShadowsApplied = &shadowTex->applied;

	uiModule = new UIModule("UIModule", resolution, database);
	uiModule->LinkShaders();
	uiModule->Initialise();

	gameText = new GameText("GameText", resolution, camera);
	gameText->LinkShaders();
	gameText->Initialise();

	scoreCounter = new ScoreCounter("ScoreCounter", resolution, database);
	scoreCounter->LinkShaders();
	scoreCounter->Initialise();
	scoreCounter->paintTrailTexture = &paintTrail->paintTrailTexture;

	wireframe = new Wireframe("Wireframe", resolution, camera);
	wireframe->LinkShaders();
	wireframe->Initialise();

	gBuffer->SetReflectionTextureId(skybox->textureID);
}

void PipelineConfiguration::BuildPipeline(GraphicsPipeline* pipeline)
{
	pipeline->AddModule(paintTrail);
	pipeline->AddModule(gBuffer);
	pipeline->AddModule(skybox);
	pipeline->AddModule(shadowTex);
	pipeline->AddModule(ssao);
	pipeline->AddModule(bpLighting);
	pipeline->AddModule(uiModule);
	pipeline->AddModule(gameText);
	pipeline->AddModule(scoreCounter);
	pipeline->AddModule(wireframe);
}
