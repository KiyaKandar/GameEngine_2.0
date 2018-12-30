#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "GL/glew.h"
#include "GL/wglew.h"

#include <SOIL.h>

#include "../Utilities/Maths/Vector4.h"
#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Vector2.h"
#include "../Utilities/Maths/Quaternion.h"
#include "../Utilities/Maths/Matrix4.h"

#include "../Shaders/Shader.h"		//Students make this file...

using std::vector;

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")

#ifdef _DEBUG
#define GL_BREAKPOINT glUniform4uiv(0,0,0);//Invalid, but triggers gdebugger ;)
#else
#define GL_BREAKPOINT //
#endif

//#define OPENGL_DEBUGGING

static const float biasValues[16] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
static const NCLMatrix4 biasMatrix(const_cast<float*>(biasValues));

enum DebugDrawMode
{
	DEBUGDRAW_ORTHO,
	DEBUGDRAW_PERSPECTIVE
};

struct DebugDrawData
{
	vector<NCLVector3> lines;
	vector<NCLVector3> colours;

	GLuint array;
	GLuint buffers[2];

	DebugDrawData();
	void Draw() const;

	~DebugDrawData()
	{
		glDeleteVertexArrays(1, &array);
		glDeleteBuffers(2, buffers);
	}

	inline void Clear()
	{
		lines.clear();
		colours.clear();
	}

	inline void AddLine(const NCLVector3& from, const NCLVector3& to, const NCLVector3& fromColour,
		const NCLVector3& toColour)
	{
		lines.push_back(from);
		lines.push_back(to);

		colours.push_back(fromColour);
		colours.push_back(toColour);
	}
};

class Shader;

class OGLRenderer
{
public:
	OGLRenderer(HWND windowHandle, NCLVector2 size);
	virtual ~OGLRenderer(void);

	virtual void RenderScene() = 0;

	virtual void UpdateScene(const float& msec)
	{
	}

	void SwapBuffers() const;

	bool HasInitialised() const;

	static void DrawDebugLine(DebugDrawMode mode, const NCLVector3& from, const NCLVector3& to,
		const NCLVector3& fromColour = NCLVector3(1, 1, 1), const NCLVector3& toColour = NCLVector3(1, 1, 1));
	static void DrawDebugBox(DebugDrawMode mode, const NCLVector3& at, const NCLVector3& scale,
		const NCLVector3& colour = NCLVector3(1, 1, 1));
	static void DrawDebugCross(DebugDrawMode mode, const NCLVector3& at, const NCLVector3& scale,
		const NCLVector3& colour = NCLVector3(1, 1, 1));
	static void DrawDebugCircle(DebugDrawMode mode, const NCLVector3& at, const float radius,
		const NCLVector3& colour = NCLVector3(1, 1, 1));

	void SetAsDebugDrawingRenderer()
	{
		debugDrawingRenderer = this;
	}

	Shader* GetCurrentShader() const
	{
		return currentShader;
	}

	NCLMatrix4 GetViewMatrix() const
	{
		return viewMatrix;
	}

	NCLMatrix4 GetProjectionMatrix() const
	{
		return projMatrix;
	}

	NCLMatrix4 GetModelMatrix() const
	{
		return modelMatrix;
	}

	NCLMatrix4 GetTextureMatrix() const
	{
		return textureMatrix;
	}

	virtual void Resize(int x, int y);

protected:
	void UpdateShaderMatrices();
	void UpdateShaderMatrices(Shader* shader);
	void SetCurrentShader(Shader* s);

	void SetTextureRepeating(GLuint target, bool state) const;

	void DrawDebugPerspective(NCLMatrix4* matrix = 0);
	void DrawDebugOrtho(NCLMatrix4* matrix = 0);

	Shader* currentShader;

	NCLMatrix4 projMatrix; //Projection matrix
	NCLMatrix4 modelMatrix; //Model matrix. NOT MODELVIEW
	NCLMatrix4 viewMatrix; //View matrix
	NCLMatrix4 textureMatrix; //Texture matrix

	int width; //Render area width (not quite the same as window width)
	int height; //Render area height (not quite the same as window height)
	bool init; //Did the renderer initialise properly?

	HDC deviceContext; //...Device context?
	HGLRC renderContext; //Permanent Rendering Context

	static DebugDrawData* orthoDebugData;
	static DebugDrawData* perspectiveDebugData;

	static OGLRenderer* debugDrawingRenderer;
	static Shader* debugDrawShader;

#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLuint source, GLuint type, GLuint id, GLuint severity,
		int length, const char* message, void* userParam);
#endif

	static bool drawnDebugOrtho;
	static bool drawnDebugPerspective;
};
