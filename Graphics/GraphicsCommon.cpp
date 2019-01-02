#include "GraphicsCommon.h"

NCLMatrix4 CommonGraphicsData::SHARED_PROJECTION_MATRIX =
	NCLMatrix4::Perspective(1.0f, 150000.0f, 1280.0f / 720.0f, 60.0f);

NCLMatrix4 CommonGraphicsData::SHARED_ORTHOGRAPHIC_MATRIX =
	NCLMatrix4::Orthographic(-1.0f, 1.0f, 1264 / 2.0f, -1264 / 2.0f, 681 / 2.0f, -681 / 2.0f);
