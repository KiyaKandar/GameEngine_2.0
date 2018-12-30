#include "LegacyMesh.h"

LegacyMesh::LegacyMesh(void)
{
	for (int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	texture = 0;
	numIndices = 0;
	bumpTexture = 0;

	vertices = NULL;
	colours = NULL;
	normals = NULL;
	textureCoords = NULL;
	indices = NULL;
	tangents = NULL;
	type = GL_TRIANGLES;
}

LegacyMesh::~LegacyMesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	//glDeleteTextures(1, &bumpTexture);
	//glDeleteTextures(1, &texture);
	delete[] vertices;
	delete[] colours;
	delete[] normals;
	delete[] textureCoords;
	delete[] indices;
	delete[] tangents;
}

LegacyMesh* LegacyMesh::GenerateTriangle()
{
	LegacyMesh* m = new LegacyMesh();
	m->numVertices = 3;

	m->vertices = new NCLVector3[m->numVertices];
	m->vertices[0] = NCLVector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = NCLVector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = NCLVector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new NCLVector2[m->numVertices];
	m->textureCoords[0] = NCLVector2(0.5f, 0.0f);
	m->textureCoords[1] = NCLVector2(1.0f, 1.0f);
	m->textureCoords[2] = NCLVector2(0.0f, 1.0f);

	m->colours = new NCLVector4[m->numVertices];

	m->colours[0] = NCLVector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = NCLVector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = NCLVector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

LegacyMesh* LegacyMesh::GenerateQuad()
{
	LegacyMesh* m = new LegacyMesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new NCLVector3[m->numVertices];
	m->textureCoords = new NCLVector2[m->numVertices];
	m->colours = new NCLVector4[m->numVertices];
	m->normals = new NCLVector3[m->numVertices];
	m->tangents = new NCLVector3[m->numVertices];

	m->vertices[0] = NCLVector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = NCLVector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = NCLVector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = NCLVector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = NCLVector2(0.0f, 1.0f);
	m->textureCoords[1] = NCLVector2(0.0f, 0.0f);
	m->textureCoords[2] = NCLVector2(1.0f, 1.0f);
	m->textureCoords[3] = NCLVector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		m->colours[i] = NCLVector4(1.0f, 1.0f, 1.0f, 1.0f);
		m->normals[i] = NCLVector3(0.0f, 0.0f, -1.0f);
		m->tangents[i] = NCLVector3(1.0f, 0.0f, 1.0f);
	}

	m->BufferData();
	return m;
}

void LegacyMesh::SetColour(NCLVector4 newCol, LegacyMesh* m) const
{
	m->colours = new NCLVector4[m->numVertices];
	for (unsigned i = 0; i < m->numVertices; i++)
	{
		m->colours[i] = newCol;
	}

	m->BufferData();
}

void LegacyMesh::BufferData()
{
	//bind this meshes array object, now any vertex array functionality will be performed on the newly bund array object
	glBindVertexArray(arrayObject);

	//generate new VBO, store its name in first index of our bufferObject array
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	//bind it, so all vertex buffer functions will be applied to our new VBO,  and also assigns new VBO to to currently bound VAO, which will tie all VBOs into our VAO
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	//now buffer the vertex position data, which copies data into graphics memory, second param tells how much memory we need in graphics memory to store this
	//and the pointer to the data to pass, finally inform OpenGL how we expect data to be used, either dynamically updated, or loaded once as static data
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(NCLVector3), vertices, GL_STATIC_DRAW);
	//now VBO data copied into memory, set how to access it, by modifying VAO.
	//tell OpenGL vertex attrib has 3 float components per vertex
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//next lines enables this
	glEnableVertexAttribArray(VERTEX_BUFFER);

	//now do same for textureCoords
	if (textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(NCLVector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}
	//now do the same for colours
	if (colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(NCLVector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	if (normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(NCLVector3), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}
	if (tangents)
	{
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(NCLVector3), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}
	if (indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void LegacyMesh::GenerateNormals()
{
	if (!normals)
	{
		normals = new NCLVector3[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i] = NCLVector3();
	}

	if (indices)
	{
		//Generate per-vertex normals
		for (GLuint i = 0; i < numIndices; i += 3)
		{
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			NCLVector3 normal = NCLVector3::Cross(
				(vertices[b] - vertices[a]),
				(vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else
	{
		//Simply list of triangles, so generate face normals
		for (GLuint i = 0; i < numVertices; i += 3)
		{
			NCLVector3& a = vertices[i];
			NCLVector3& b = vertices[i + 1];
			NCLVector3& c = vertices[i + 2];

			NCLVector3 normal = NCLVector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i = 1] = normal;
			normals[i + 2] = normal;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i].Normalise();
	}
}

void LegacyMesh::GenerateTangents()
{
	if (!tangents)
	{
		tangents = new NCLVector3[numVertices];
	}

	if (!textureCoords)
	{
		textureCoords = new NCLVector2[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i] = NCLVector3();
	}

	if (indices)
	{
		for (GLuint i = 0; i < numIndices; i += 3)
		{
			int a = indices[i];
			int b = indices[i + 1];
			int c = indices[i + 2];

			NCLVector3 tangent = GenerateTangent(vertices[a], vertices[b],
				vertices[c], textureCoords[a],
				textureCoords[b], textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else
	{
		//Simply list of triangles, so generate face normals
		for (GLuint i = 0; i < numVertices; i += 3)
		{
			NCLVector3 tangent = GenerateTangent(vertices[i], vertices[i + 1],
				vertices[i + 2], textureCoords[i],
				textureCoords[i + 1], textureCoords[i + 2]);

			tangents[i] += tangent;
			tangents[i + 1] += tangent;
			tangents[i + 2] += tangent;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i].Normalise();
	}
}

NCLVector3 LegacyMesh::GenerateTangent(const NCLVector3& a, const NCLVector3& b,
	const NCLVector3& c, const NCLVector2& ta,
	const NCLVector2& tb, const NCLVector2& tc) const
{
	NCLVector2 coord1 = tb - ta;
	NCLVector2 coord2 = tc - ta;

	NCLVector3 vertex1 = b - a;
	NCLVector3 vertex2 = c - a;

	NCLVector3 axis = NCLVector3(vertex1 * coord2.y - vertex2 * coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}

void LegacyMesh::Draw(Shader& shader, NCLMatrix4& worldTransform)
{
	glUniform1i(glGetUniformLocation(shader.GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "modelMatrix"), 1, false, (float*)&worldTransform);

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
