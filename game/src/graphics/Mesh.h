/*
FILE: Mesh.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Texture.h"
#include "Shaders.h"

#include "GL\glew.h"
#include <glm/glm.hpp>
#include <vector>



class Mesh
{
public:
	///
	// Vertex sub-class
	///

	struct Vertice
	{
		Vertice(float x, float y, float z, float r, float g, float b, float a, float texX, float texY)
			: data{ x,y,z,r,g,b,a,texX, 1 - texY } {} //Flip y because image is loaded upside down

		const static int vertDataVars = 3 + 4 + 2; // Number of variables a vertice holds
		float data[vertDataVars];
	};
	
	///
	// Mesh constructor/deconstructor
	///

	Mesh();
	~Mesh();

	///
	// Mesh construction functions
	///

	void AddVertex(float x, float y, float z, float r, float g, float b, float a, float texX, float texY);
	void AddTriangle(
		float x1, float y1, float z1, float r1, float g1, float b1, float a1, float texX1, float texY1,
		float x2, float y2, float z2, float r2, float g2, float b2, float a2, float texX2, float texY2,
		float x3, float y3, float z3, float r3, float g3, float b3, float a3, float texX3, float texY3);
	std::vector<Vertice>* GetVertices();
	void CompileMesh(); // Creates mesh with provided vertices

	///
	// Render functions
	///

	void BindVAO() { glBindVertexArray(vaoID); }
	void BindVBO() { glBindBuffer(GL_ARRAY_BUFFER, vboID); }
	int NumVerts() { return (int)vertices.size(); }

	///
	// Static functions
	///

	static void BindInstanceVBO() { glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); }
	static void BindTextureVBO() { glBindBuffer(GL_ARRAY_BUFFER, textureVBO); }

private:
	// Static buffers
	static GLuint instanceVBO, textureVBO;

	// Per mesh buffers
	GLuint vaoID, vboID;

	// Vertex data
	std::vector<Vertice> vertices;
};

