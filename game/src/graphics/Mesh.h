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



enum BlendMode
{
	BM_DISABLED, BM_DEFAULT, BM_DISABLE_ALPHA, BM_ADDITIVE
};

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

	Mesh(GLenum renderMode = GL_TRIANGLES);
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
	// Non-Animated Texture functions
	///

	void SetTexture(Texture* tex);
	Texture* GetTexture() const { return texture; }
	
	/* Returns texture/animated texture id that will be rendered. *
	 * Returns default texture is no texture is set.              */
	GLuint GetRenderTextureID(); 

	///
	// Animated Texture functions
	///
	void SetTexture(AnimatedTexture* at, float fps) { animatedTexture = at; AT_fps = fps; }
	void SetAnimatedTexture(AnimatedTexture* at, float fps) { animatedTexture = at; AT_fps = fps; }
	AnimatedTexture* GetAnimatedTexture() const { return animatedTexture; }

	void UpdateAnimatedTexture(float dt);
	void SetFrame(int frame) { AT_frame = frame; AT_timer = 0; }
	bool IsAnimated() const { return animatedTexture != nullptr; }

	///
	// Render functions
	///

	void SetRenderData(glm::mat4 matrix, std::vector<float>* data);
	void BindVAO() { glBindVertexArray(vaoID); }
	void BindVBO() { glBindBuffer(GL_ARRAY_BUFFER, vboID); }
	int NumVerts() { return (int)vertices.size(); }

	///
	// Static functions
	///

	static void BindInstanceVBO() { glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); }
	static void BindTextureVBO() { glBindBuffer(GL_ARRAY_BUFFER, textureVBO); }
	static void UseBlendMode(BlendMode bm);

private:

	// Static buffers
	static GLuint instanceVBO, textureVBO;

	// Per mesh buffers
	GLuint vaoID, vboID;

	// Vertex data
	std::vector<Vertice> vertices;

	// Non-Animated Texture
	Texture* texture = nullptr;
	
	// Animated Texture
	AnimatedTexture* animatedTexture = nullptr;
	int AT_frame = 0;
	float AT_fps, AT_timer = 0;
};

