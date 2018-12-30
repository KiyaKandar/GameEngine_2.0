#include "Renderer.h"

#include "../../Input/Devices/Window.h"
#include "../GraphicsUtility.h"
#include "../Utility/Camera.h"
#include "../Utilities/Maths/Matrix4.h"
#include "../Resource Management/Database/Database.h"
#include "../Gameplay/GameObject.h"

Renderer::Renderer() : OGLRenderer(0, NCLVector2())
{
	window = nullptr;
	camera = nullptr;
	resolution = NCLVector2();
	pipeline = GraphicsPipeline(nullptr);
}

Renderer::Renderer(GameTimer* parentTimer, Window* window, Camera* camera)
	: OGLRenderer(window->GetHandle(), window->GetScreenSize())
{
	this->window = window;
	this->camera = camera;
	this->resolution = window->GetScreenSize();
	this->parentTimer = parentTimer;

	parentTimer->AddChildTimer("Update Scene Management");
	parentTimer->AddChildTimer("Render Modules");

	pipeline = GraphicsPipeline(parentTimer->GetChildTimer("Render Modules"));

	globalOrthographicMatrix = NCLMatrix4::Orthographic(-1.0f,10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);

	loadingScreenMesh = new SceneNode("../Data/Resources/Meshes/cube.obj", NCLVector4(1,0,0,1));
	loadingScreenMesh->GetMesh()->LoadTexture("../Data/Resources/Textures/loadingtexture.png");
	loadingScreenMesh->GetMesh()->SetupMesh();
	loadingScreenMesh->SetTransform(NCLVector3(0, 0, -10));
	loadingScreenMesh->SetModelScale(NCLVector3(1, 1, 1));
	loadingScreenMesh->Update(0.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	loadingScreenShader = new Shader(SHADERDIR"/basicVertex.glsl", SHADERDIR"/basicFrag.glsl");
	loadingScreenShader->LinkProgram();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	GraphicsUtility::CheckGLError("Renderer Initialisation");
}

Renderer::~Renderer()
{
	delete loadingScreenMesh;
	delete loadingScreenShader;
}

void Renderer::RenderLoadingScreen(const float& deltatime)
{
	camera->SetPosition(NCLVector3(0, 0, 0));
	camera->SetPitch(0);
	camera->SetYaw(0);

	loadingScreenMesh->SetTransform(loadingScreenMesh->GetTransform()
		* NCLMatrix4::Rotation(5.0f, NCLVector3(0,1,0)));

	loadingScreenMesh->Update(deltatime);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(loadingScreenShader->GetProgram());
	viewMatrix = camera->BuildViewMatrix();

	glUniform4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "colour"), 1, (float*)&loadingScreenMesh->getColour());
	glUniformMatrix4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);
	glUniform1i(glGetUniformLocation(loadingScreenShader->GetProgram(), "hasTexture"), 1);

	loadingScreenMesh->Draw(*loadingScreenShader);
	SwapBuffers();
}

void Renderer::Initialise(SceneManager* sceneManager, Database* database)
{
	graphicsConfig = PipelineConfiguration(sceneManager, window, camera, resolution);
	graphicsConfig.InitialiseModules(database);
	graphicsConfig.BuildPipeline(&pipeline);

	XMLParser graphicsconfigParser;
	graphicsconfigParser.LoadXmlFile("../Data/Resources/Graphics Config/graphicsConfigXML.xml");
	Node* node = graphicsconfigParser.parsedXml;

	for (size_t i = 0; i < node->children.size(); i++)
	{
		std::string enabled = node->children[i]->value;
		std::string graphicsModuleName = node->children[i]->name;

		if (enabled == "Enabled")
		{
			pipeline.ToggleModule(graphicsModuleName, true);
		}
		else
		{
			pipeline.ToggleModule(graphicsModuleName, false);
		}
	}

	this->sceneManager = sceneManager;
}

void Renderer::Update(const float& deltatime)
{
	UpdateScene(deltatime);
	RenderScene();
}

void Renderer::AddSceneNode(SceneNode* sceneNode)
{
	(*sceneManager->GetAllNodes())->push_back(sceneNode);
}

void Renderer::RemoveSceneNodeByResourceName(std::string resourcename)
{
	for (auto sceneNodeIterator = (*sceneManager->GetAllNodes())->begin(); sceneNodeIterator != (*sceneManager->GetAllNodes())->end(); ++ sceneNodeIterator)
	{
		if ((*sceneNodeIterator)->GetParent()->GetName() == resourcename)
		{
			(*sceneManager->GetAllNodes())->erase(sceneNodeIterator);
			break;
		}
	}
}

void Renderer::ToggleModule(const std::string& moduleName, bool enabled)
{
	pipeline.ToggleModule(moduleName, enabled);
}

GraphicsModule* Renderer::GetGraphicsModule(const std::string& moduleName)
{
	return pipeline.GetGraphicsModule(moduleName);
}

void Renderer::UpdateScene(const float& msec)
{
	parentTimer->BeginChildTimedSection("Update Scene Management");

	camera->UpdateCamera();
	camera->BuildViewMatrix();
	camera->UpdateViewFrustum(CommonGraphicsData::SHARED_PROJECTION_MATRIX);
	pipeline.UpdateModules(msec);
	sceneManager->ClearMeshLists();
	sceneManager->BuildMeshLists();

	parentTimer->EndChildTimedSection("Update Scene Management");
}

void Renderer::RenderScene()
{
	parentTimer->BeginChildTimedSection("Render Modules");

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	pipeline.ExecuteModules();
	SwapBuffers();

	parentTimer->EndChildTimedSection("Render Modules");
}
