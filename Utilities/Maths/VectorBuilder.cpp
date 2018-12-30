#include "VectorBuilder.h"
#include "Vector3.h"
#include "Vector4.h"
#include <random>

float VectorBuilder::GetVectorComponentFromNode(const Node* node, float* min, float* max)
{
	if (node->nodeType == "random")
	{
		*min = std::stof(node->children[0]->value);
		*max = std::stof(node->children[1]->value);
		return GetRandomVectorComponent(*min, *max);
	}
	else
	{
		if (min != nullptr)
		{
			*min = 0.0f;
		}
		if (max != nullptr)
		{
			*max = 0.0f;
		}
		return std::stof(node->value);
	}
}

float VectorBuilder::GetRandomVectorComponent(const float mi, const float ma)
{
	std::random_device rd;
	std::mt19937 rng(rd()); // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni((int)mi, (int)ma); // guaranteed unbiased
	auto r1 = uni(rng);
	return (float)r1;
}

NCLVector3 VectorBuilder::BuildVector3(Node* node)
{
	const float x = VectorBuilder::GetVectorComponentFromNode(node->children[0]);
	const float y = VectorBuilder::GetVectorComponentFromNode(node->children[1]);
	const float z = VectorBuilder::GetVectorComponentFromNode(node->children[2]);

	return NCLVector3(x, y, z);
}

NCLVector3 VectorBuilder::BuildVector3(std::string text)
{
	std::stringstream ss(text);
	std::string item;
	std::vector<std::string> tokens;

	while (std::getline(ss, item, ','))
	{
		tokens.push_back(item);
	}

	return NCLVector3(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
}

NCLVector4 VectorBuilder::BuildVector4(Node* node)
{
	const float x = VectorBuilder::GetVectorComponentFromNode(node->children[0]);
	const float y = VectorBuilder::GetVectorComponentFromNode(node->children[1]);
	const float z = VectorBuilder::GetVectorComponentFromNode(node->children[2]);
	const float w = VectorBuilder::GetVectorComponentFromNode(node->children[3]);

	return NCLVector4(x, y, z, w);
}

NCLVector4 VectorBuilder::BuildVector4(std::string text)
{
	std::stringstream ss(text);
	std::string item;
	std::vector<std::string> tokens;

	while (std::getline(ss, item, ','))
	{
		tokens.push_back(item);
	}

	return NCLVector4(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
}
