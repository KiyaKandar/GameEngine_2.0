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
	: OGLRenderer(window->getHandle(), window->getScreenSize())
{
	this->window = window;
	this->camera = camera;
	this->resolution = window->getScreenSize();
	this->parentTimer = parentTimer;

	parentTimer->addChildTimer("Update Scene Management");
	parentTimer->addChildTimer("Render Modules");

	pipeline = GraphicsPipeline(parentTimer->getChildTimer("Render Modules"));

	globalOrthographicMatrix = NCLMatrix4::orthographic(-1.0f,10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);

	loadingScreenMesh = new SceneNode("../Data/Resources/Meshes/cube.obj", NCLVector4(1,0,0,1));
	loadingScreenMesh->GetMesh()->loadTexture("../Data/Resources/Textures/loadingtexture.png");
	loadingScreenMesh->GetMesh()->setupMesh();
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

void Renderer::renderLoadingScreen(const float& deltatime)
{
	camera->setPosition(NCLVector3(0, 0, 0));
	camera->setPitch(0);
	camera->setYaw(0);

	loadingScreenMesh->SetTransform(loadingScreenMesh->GetTransform()
		* NCLMatrix4::rotation(5.0f, NCLVector3(0,1,0)));

	loadingScreenMesh->Update(deltatime);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(loadingScreenShader->GetProgram());
	viewMatrix = camera->buildViewMatrix();

	glUniform4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "colour"), 1, (float*)&loadingScreenMesh->getColour());
	glUniformMatrix4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(loadingScreenShader->GetProgram(), "projMatrix"), 1, false, (float*)&CommonGraphicsData::SHARED_PROJECTION_MATRIX);
	glUniform1i(glGetUniformLocation(loadingScreenShader->GetProgram(), "hasTexture"), 1);

	loadingScreenMesh->Draw(*loadingScreenShader);
	swapBuffers();
}

void Renderer::initialise(SceneManager* sceneManager, Database* database)
{
	graphicsConfig = PipelineConfiguration(sceneManager, window, camera, resolution);
	graphicsConfig.initialiseModules(database);
	graphicsConfig.buildPipeline(&pipeline);

	XMLParser graphicsconfigParser;
	graphicsconfigParser.loadXMLFile("../Data/Resources/Graphics Config/graphicsConfigXML.xml");
	Node* node = graphicsconfigParser.parsedXml;

	for (size_t i = 0; i < node->children.size(); i++)
	{
		std::string enabled = node->children[i]->value;
		std::string graphicsModuleName = node->children[i]->name;

		if (enabled == "Enabled")
		{
			pipeline.toggleModule(graphicsModuleName, true);
		}
		else
		{
			pipeline.toggleModule(graphicsModuleName, false);
		}
	}

	this->sceneManager = sceneManager;
}

void Renderer::update(const float& deltatime)
{
	updateScene(deltatime);
	renderScene();
}

void Renderer::addSceneNode(SceneNode* sceneNode)
{
	(*sceneManager->getAllNodes())->push_back(sceneNode);
}

void Renderer::removeSceneNodeByResourceName(std::string resourcename)
{
	for (auto sceneNodeIterator = (*sceneManager->getAllNodes())->begin(); sceneNodeIterator != (*sceneManager->getAllNodes())->end(); ++ sceneNodeIterator)
	{
		if ((*sceneNodeIterator)->getParent()->getName() == resourcename)
		{
			(*sceneManager->getAllNodes())->erase(sceneNodeIterator);
			break;
		}
	}
}

void Renderer::toggleModule(const std::string& moduleName, bool enabled)
{
	pipeline.toggleModule(moduleName, enabled);
}

GraphicsModule* Renderer::getGraphicsModule(const std::string& moduleName)
{
	return pipeline.getGraphicsModule(moduleName);
}

void Renderer::updateScene(const float& msec)
{
	parentTimer->beginChildTimedSection("Update Scene Management");

	camera->updateCamera();
	camera->buildViewMatrix();
	camera->updateViewFrustum(CommonGraphicsData::SHARED_PROJECTION_MATRIX);
	pipeline.updateModules(msec);
	sceneManager->clearMeshLists();
	sceneManager->buildMeshLists();

	parentTimer->endChildTimedSection("Update Scene Management");
}

void Renderer::renderScene()
{
	parentTimer->beginChildTimedSection("Render Modules");

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	pipeline.executeModules();
	swapBuffers();

	parentTimer->endChildTimedSection("Render Modules");
}
