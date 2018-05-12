#pragma once
#include "../../Resource Management/XMLParser.h"
#include <random>

class NCLVector3;
class NCLVector4;

class VectorBuilder 
{
public:
	static float getVectorComponentFromNode(const Node* node, float* min = nullptr, float* max = nullptr);
	static float getRandomVectorComponent(const float mi, const float ma);

	static NCLVector3 buildVector3(Node* node);
	static NCLVector3 buildVector3(std::string text);

	static NCLVector4 buildVector4(Node* node);
	static NCLVector4 buildVector4(std::string text);
};