#include "Wireframe.h"
#include "../../Utility/Camera.h"
#include "../../GraphicsUtility.h"

const int STEP_COUNT = 18;
const float DIVISOR = 360.0f / STEP_COUNT;

Wireframe::Wireframe(const std::string identifier, const NCLVector2 resolution, Camera* camera) : GraphicsModule(identifier, resolution)
{
	debugShader = new Shader(SHADERDIR"/debugVertex.glsl", SHADERDIR"/debugFragment.glsl");
	this->camera = camera;
}

Wireframe::~Wireframe()
{
	delete debugShader;
}

void Wireframe::LinkShaders()
{
	debugShader->LinkProgram();
}

void Wireframe::Initialise()
{
	
}

void Wireframe::Apply()
{
	GraphicsUtility::ClearGLErrorStack();
	
	SetCurrentShader(debugShader);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	GraphicsUtility::CheckGLError("");
	NCLMatrix4 viewMatrix = camera->BuildViewMatrix();
	NCLMatrix4 viewProjMatrix = CommonGraphicsData::SHARED_PROJECTION_MATRIX * viewMatrix;
	BuildLinesFromSpheres();

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewProjMatrix"), 1, false, (float*)&viewProjMatrix);
	
	RenderLines();

	GraphicsUtility::CheckGLError("");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Wireframe::RegenerateShaders()
{
	debugShader->Regenerate();
}

void Wireframe::AddLine(NCLVector3 from, NCLVector3 to, NCLVector3 colour)
{
	linePoints.push_back(from);
	linePoints.push_back(to);
	lineColours.push_back(colour);
	lineColours.push_back(colour);
}

void Wireframe::AddSphere(NCLVector3 position, float radius, NCLVector3 colour)
{
	spherePositions.push_back(position);
	sphereColours.push_back(colour);
	radii.push_back(radius);
}

void Wireframe::LocateUniforms()
{
}

void Wireframe::BuildLinesFromSpheres()
{
	for (int j = 0; j < (int)spherePositions.size(); ++j)
	{
		SplitSphere(j);
	}

	sphereColours.clear();
	spherePositions.clear();
	radii.clear();
}

void Wireframe::SplitSphere(int circleIndex)
{
	for (int i = 0; i < STEP_COUNT; ++i)
	{
		float startx = radii[circleIndex] * (float)cos(DegToRad(i * DIVISOR)) + spherePositions[circleIndex].x;
		float endx = radii[circleIndex] * (float)cos(DegToRad((i + 1) * DIVISOR)) + spherePositions[circleIndex].x;

		float startz = radii[circleIndex] * (float)cos(DegToRad(i * DIVISOR)) + spherePositions[circleIndex].z;
		float endz = radii[circleIndex] * (float)cos(DegToRad((i + 1) * DIVISOR)) + spherePositions[circleIndex].z;

		float starty = radii[circleIndex] * (float)sin(DegToRad(i * DIVISOR)) + spherePositions[circleIndex].y;
		float endy = radii[circleIndex] * (float)sin(DegToRad((i + 1) * DIVISOR)) + spherePositions[circleIndex].y;

		AddLine(NCLVector3(startx, starty, spherePositions[circleIndex].z),
			NCLVector3(endx, endy, spherePositions[circleIndex].z), sphereColours[circleIndex]);

		AddLine(NCLVector3(spherePositions[circleIndex].x, starty, startz),
			NCLVector3(spherePositions[circleIndex].x, endy, endz), sphereColours[circleIndex]);
	}
}

void Wireframe::RenderLines()
{
	if (!linePoints.empty())
	{
		glGenVertexArrays(1, &array);
		glBindVertexArray(array);
		glGenBuffers(2, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, linePoints.size() * sizeof(NCLVector3), &linePoints[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, lineColours.size() * sizeof(NCLVector3), &lineColours[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(array);
		glDrawArrays(GL_LINES, 0, linePoints.size());
		glDeleteBuffers(2, buffers);

		glBindVertexArray(0);

		linePoints.clear();
		lineColours.clear();
	}
}