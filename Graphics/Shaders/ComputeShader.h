#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include "../Utilities/Maths/Vector3.h"

using namespace std;

class ComputeShader
{
public:
	ComputeShader(string compute, bool isVerbose = false);
	~ComputeShader();

	GLuint GetProgram() const
	{
		return program;
	}

	void UseProgram() const;
	bool LinkProgram() const;

	void Compute(NCLVector3 workGroups) const;

	void Regenerate();
protected:
	bool LoadShaderFile(string from, string& into);
	string IncludeShader(string includeLine);
	GLuint GenerateShader(string from);

	GLuint object[1];
	GLuint program;

	bool loadFailed;
	bool verbose;

	string compute;
};
