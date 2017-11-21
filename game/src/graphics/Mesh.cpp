/*
FILE: Mesh.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Mesh.h"
#include "Texture.h"

#include "glm\glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>


///
// Mesh
///

// Statics 
GLuint Mesh::instanceVBO = 0;
GLuint Mesh::textureVBO = 0;
static Texture* defaultTexture;

Mesh::Mesh()
{
	// Generate Vertex Attribute Object (VAO)
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Instance VBOs are static, make them if they dont exist
	if (!instanceVBO)
	{
		// Generate buffers to hold vertex and texture data
		glGenBuffers(1, &instanceVBO);
		glGenBuffers(1, &textureVBO);
	}
	
	// Setup VBOs for this VAO
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	Shaders::spriteShader->ApplyAttributes(3, 9); // Apply instance attributes

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	Shaders::spriteShader->ApplyAttributes(9, 10); // Apply texture instance attribute

	// VBO for non chaning vertex data
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	Shaders::spriteShader->ApplyAttributes(0,3); // Apply non-instance attributes for vao to remember
}

Mesh::~Mesh()
{
	//glDeleteBuffers(1, &vboID);
}

void Mesh::AddVertex(float x, float y, float z, float r, float g, float b, float a, float s, float t)
{
	Vertice vert = Vertice{ x, y, z, r, g, b, a, s, t };
	vertices.push_back(vert);
}

void Mesh::AddTriangle(
	float x1, float y1, float z1, float r1, float g1, float b1, float a1, float s1, float t1,
	float x2, float y2, float z2, float r2, float g2, float b2, float a2, float s2, float t2,
	float x3, float y3, float z3, float r3, float g3, float b3, float a3, float s3, float t3)
{
	AddVertex(x1, y1, z1, r1, g1, b1, a1, s1, t1);
	AddVertex(x2, y2, z2, r2, g2, b2, a2, s2, t2);
	AddVertex(x3, y3, z3, r3, g3, b3, a3, s3, t3);
}

std::vector<Mesh::Vertice>* Mesh::GetVertices()
{
	return &vertices;
}

void Mesh::CompileMesh()
{
	glBindVertexArray(vaoID); // Enable vao to ensure correct mesh is compiled
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Enable vbo
	
	int s = (int)vertices.size() * Vertice::vertDataVars;
	float* verts = new float[s];
	
	int i = 0;
	for(auto vert : vertices)
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j, ++i)
			verts[i] = vert.data[j];

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * s, verts, GL_STATIC_DRAW); //Give vbo verts
	delete [] verts;
}

