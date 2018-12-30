#include "SubMesh.h"

SubMesh::SubMesh(vector<Vertex> vertices, vector<unsigned int> indices,
	vector<Texture> textures, vector<Texture> heights, std::vector<VertexBoneData> bones,
	BoundingBox AABB, int numTransforms)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->heights = heights;
	this->bones = bones;

	box = AABB;
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	baseColour = NCLVector4(0.6f, 0.6f, 0.6f, 1.0f);

	//SetupMesh();

	CalculateBoundingRadius();
}

SubMesh::SubMesh()
{
}

SubMesh::~SubMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void SubMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &BBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//VERTEX DATA
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	//Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	//Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	//Vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	//Vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//ANIMATION DATA
	glBindBuffer(GL_ARRAY_BUFFER, BBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(NUM_BONES_PER_VEREX * 4));

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//INDICES
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void SubMesh::Draw(Shader& shader, NCLMatrix4& worldTransform)
{
	//Bind all textures of the mesh
	for (unsigned int i = 1; i <= textures.size(); i++)
	{
		//Activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), textures[i - 1].type.c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i - 1].id);
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "modelMatrix"), 1, false, (float*)&worldTransform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void SubMesh::Draw(Shader& shader)
{
	//Bind all textures of the mesh
	for (unsigned int i = 1; i <= textures.size(); i++)
	{
		//Activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), textures[i - 1].type.c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i - 1].id);
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "modelMatrix"), 1, false, (float*)&transform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void SubMesh::DrawShadow(Shader& shader, NCLMatrix4& worldTransform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "modelMatrix"), 1, false, (float*)&worldTransform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void SubMesh::AddTexture(unsigned int texture)
{
	Texture text;
	text.type = "diffuseTex";
	text.id = texture;
	textures.push_back(text);
}