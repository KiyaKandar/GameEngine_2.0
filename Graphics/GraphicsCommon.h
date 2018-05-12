#pragma once

#define SHADERDIR	"../Shaders/"
#define MESHDIR		"../Data/Resources/Meshes/"
#define TEXTUREDIR  "../Data/Resources/Textures/"
#define SOUNDSDIR	"../Data/Resources/Sounds/"

#include "Maths/Matrix4.h"

class CommonGraphicsData
{
public:
	enum GBUFFER
	{
		GPOSITION,
		GNORMAL,
		GALBEDO
	};

	enum AMBIENT_TEX
	{
		SSAO_INDEX
	};

	static NCLMatrix4 SHARED_PROJECTION_MATRIX;
	static NCLMatrix4 SHARED_ORTHOGRAPHIC_MATRIX;
};