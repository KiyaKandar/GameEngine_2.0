//#include "GPUCloth.h"
//
//#include <vector>
//
//GPUCloth::GPUCloth(ClothGrid clothGrid)
//{
//	this->clothGrid = clothGrid;
//
//	velocityPrep = new ComputeShader(
//		"../../Data/Shaders/ComputeShaders/velocityPrep.glsl", true);
//	velocityPrep->LinkProgram();
//
//	nodes = new NodeData[clothGrid.numNodes];
//	pairs = new Vector4[clothGrid.numNodes];
//	inertias = new Matrix3[clothGrid.numNodes];
//	velocities = new Vector4[clothGrid.numNodes];
//	restDistances = new float[clothGrid.numNodes * 3];
//
//	PrepareData();
//}
//
//
//GPUCloth::~GPUCloth()
//{
//	delete velocityPrep;
//	delete[] nodes;
//	delete[] pairs;
//	delete[] inertias;
//	delete[] restDistances;
//	delete[] velocities;
//}
//
//void GPUCloth::PrepareData()
//{
//	unsigned int i = 0;
//
//	for (GameObject* node : clothGrid.grid)
//	{
//		NodeData nodeData;
//
//		nodeData.inverseMass = Vector4(node->Physics()->GetInverseMass(), 1.0f, 1.0f, 1.0f);
//		inertias[i] = node->Physics()->GetInverseInertia();
//
//		Vector3 position = node->Physics()->GetPosition();
//		nodeData.position = Vector4(position.x, position.y, position.z, 1.0f);
//
//		Vector3 linVel = node->Physics()->GetLinearVelocity();
//		velocities[i] = Vector4(linVel.x, linVel.y, linVel.z, 1.0f);
//
//		nodes[i] = nodeData;
//
//		Vector4 connectedNodes(-1, -1, -1, -1);
//
//		if (i % int(clothGrid.gridSize.x) != 0) //Pair left
//		{
//			restDistances[i * 3] = Vector3(clothGrid.grid[i - 1]->Physics()->GetPosition()
//				- position).Length();
//			connectedNodes.x = i - 1;
//		}
//
//		if (i < clothGrid.gridSize.x * (clothGrid.gridSize.y - 1)) //Pair above
//		{
//			restDistances[(i * 3) + 1] = Vector3(
//				clothGrid.grid[i + clothGrid.gridSize.x]->Physics()->GetPosition()
//				- position).Length();
//			connectedNodes.y = i + clothGrid.gridSize.x;
//
//			if (i % int(clothGrid.gridSize.x) != 0) //Pair bove left
//			{
//				restDistances[(i * 3) + 2] = Vector3(
//					clothGrid.grid[i + clothGrid.gridSize.x - 1]->Physics()->GetPosition()
//					- position).Length();
//				connectedNodes.z = i + clothGrid.gridSize.x - 1;
//			}
//		}
//
//		pairs[i] = connectedNodes;
//
//		++i;
//	}
//
//	glGenBuffers(1, &dataSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(NodeData) * clothGrid.numNodes, nodes, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dataSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	glGenBuffers(1, &inertiasSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, inertiasSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix3) * clothGrid.numNodes, inertias, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, inertiasSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	glGenBuffers(1, &pairsSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pairsSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * clothGrid.numNodes, pairs, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pairsSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	glGenBuffers(1, &restDistancesSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, restDistancesSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * clothGrid.numNodes * 3, restDistances, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, restDistancesSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	glGenBuffers(1, &velocitiesSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitiesSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * clothGrid.numNodes, velocities, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, velocitiesSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//}
//
//void GPUCloth::UpdateOnGPU()
//{
//	unsigned int i = 0;
//
//	for (GameObject* node : clothGrid.grid)
//	{
//		Vector3 position = node->Physics()->GetPosition();
//		nodes[i].position = Vector4(position.x, position.y, position.z, 1.0f);
//
//		Vector3 linVel = node->Physics()->GetLinearVelocity();
//		velocities[i] = Vector4(linVel.x, linVel.y, linVel.z, 1.0f);
//
//		++i;
//	}
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(NodeData) * clothGrid.numNodes, nodes, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dataSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitiesSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * clothGrid.numNodes, velocities, GL_STATIC_COPY);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, velocitiesSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	UpdateConstraintsOnGPU();
//	UpdateNodes();
//}
//
//void GPUCloth::UpdateConstraintsOnGPU()
//{
//	velocityPrep->UseProgram();
//
//	glUniform1f(glGetUniformLocation(velocityPrep->GetProgram(), "deltatime"), 
//		PhysicsEngine::Instance()->GetDeltaTime());
//
//	velocityPrep->Compute(Vector3(clothGrid.numNodes, 1, 1));
//
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//}
// 
//void GPUCloth::UpdateNodes()
//{
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitiesSSBO);
//	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
//	memcpy(velocities, p, sizeof(Vector4) * clothGrid.numNodes);
//	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//
//	int index = 0;
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitiesSSBO);
//	for (GameObject* node : clothGrid.grid)
//	{
//		node->Physics()->SetLinearVelocity(Vector3(
//			velocities[index].x,
//			velocities[index].y,
//			velocities[index].z));
//
//		++index;
//	}
//}
