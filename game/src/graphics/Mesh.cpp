/*
FILE: Mesh.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Mesh.h"
#include "Texture.h"
#include "VertexObjects.h"

#include "glm\glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>


///
// Mesh
///
Mesh::Mesh(ShaderProgram* program)
{	
	// Setup VBOs for this VAO
	m_VAO.BindAttributesToBuffer(program, m_VertexData, 0, 3);
	m_VAO.BindAttributesToBuffer(program, m_InstanceBuffer, 3, 10);
	m_VAO.BindAttributesToBuffer(program, m_InstanceTextureBuffer, 10, 11);
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
	int s = (int)vertices.size() * Vertice::vertDataVars;
	float* verts = new float[s];
	
	int i = 0;
	for(auto vert : vertices)
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j, ++i)
			verts[i] = vert.data[j];

	m_VertexData.SetData(sizeof(float), s, verts);
	delete [] verts;
}

