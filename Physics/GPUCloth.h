//#pragma once
//
//#include "../nclgl/NCLMatrix4.h"
//#include "../nclgl/Vector4.h"
//#include "../nclgl/Vector2.h"
//#include "GameObject.h"
//#include "nclgl/ComputeShader.h"
//#include "Constraint.h"
//
//#include <vector>
//
//struct ClothGrid
//{
//	//Flipped y axis
//	vector<GameObject*> grid;
//	Vector2 gridSize;
//	vector<Vector2> constraintPairs;
//	int numNodes;
//
//	ClothGrid() { }
//
//	ClothGrid(Vector2 gridSize)
//	{
//		this->gridSize = gridSize;
//		numNodes = gridSize.x * gridSize.y;
//		grid = std::vector<GameObject*>(numNodes);
//	}
//
//	void InsertObjectAtCoordinate(GameObject* object, Vector2 coord)
//	{
//		grid[gridSize.x * coord.y + coord.x] = object;
//	}
//
//	GameObject* GetObjectAtCoordinate(Vector2 coord)
//	{
//		return grid[gridSize.x * coord.y + coord.x];
//	}
//};
//
//struct NodeData
//{
//	Vector4 position; //xyz
//	Vector4 inverseMass; //xVal
//};
//
//class GPUCloth : public Constraint
//{
//public:
//	GPUCloth(ClothGrid clothGrid);
//	~GPUCloth();
//
//	void ApplyImpulse() override
//	{
//		UpdateOnGPU();
//	}
//
//private:
//	void UpdateOnGPU();
//	void PrepareData();
//	void UpdateConstraintsOnGPU();
//	void UpdateNodes();
//
//	ClothGrid clothGrid;
//
//	NodeData* nodes;
//	Vector4* pairs;
//	Vector4* velocities;
//	Matrix3* inertias;
//	float* restDistances;
//
//	ComputeShader* velocityPrep;
//
//	GLuint dataSSBO;
//	GLuint inertiasSSBO;
//	GLuint pairsSSBO;
//	GLuint restDistancesSSBO;
//	GLuint velocitiesSSBO;
//};
//
