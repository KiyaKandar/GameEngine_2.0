#pragma once
#include "../../Resource Management/XMLParser.h"
#include <random>

class NCLVector3;
class NCLVector4;

class VectorBuilder
{
public:
	static float GetVectorComponentFromNode(const Node* node, float* min = nullptr, float* max = nullptr);
	static float GetRandomVectorComponent(const float mi, const float ma);

	static NCLVector3 BuildVector3(Node* node);
	static NCLVector3 BuildVector3(std::string text);

	static NCLVector4 BuildVector4(Node* node);
	static NCLVector4 BuildVector4(std::string text);
};
