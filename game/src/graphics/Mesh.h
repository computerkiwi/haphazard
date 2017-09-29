#pragma once

#include "Texture.h"
#include "Transform.h"
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
	struct Vertice
	{
		Vertice(float x, float y, float z, float r, float g, float b, float a, float texX, float texY)
			: data{ x,y,z,r,g,b,a,texX, 1 - texY } {} //Flip y because image is loaded upside down

		const static int vertDataVars = 3 + 4 + 2; // Number of variables a vertice holds
		float data[vertDataVars];
	};

	Mesh(GLenum renderMode = GL_TRIANGLES);
	~Mesh();

	void AddVertex(float x, float y, float z, float r, float g, float b, float a, float texX, float texY);

	void AddTriangle(
		float x1, float y1, float z1, float r1, float g1, float b1, float a1, float texX1, float texY1,
		float x2, float y2, float z2, float r2, float g2, float b2, float a2, float texX2, float texY2,
		float x3, float y3, float z3, float r3, float g3, float b3, float a3, float texX3, float texY3);

	void SetTexture(Texture* tex);
	void SetShader(ShaderProgram *shader);
	void SetBlendMode(BlendMode mode);

	std::vector<Vertice>* GetVertices();
	void CompileMesh(); // Creates mesh with provided vertices

	void SetRenderData(glm::mat4 matrix, std::vector<float>* data);

	void SetTexture(AnimatedTexture* at, float fps) { animatedTexture = at; AT_fps = fps; }
	void SetAnimatedTexture(AnimatedTexture* at, float fps) { animatedTexture = at; AT_fps = fps; }

	void SetFrame(int frame) { AT_frame = frame; AT_timer = 0; }

	void UpdateAnimatedTexture(float dt);

	void BindVAO() { glBindVertexArray(vaoID); }
	void BindVBO() { glBindBuffer(GL_ARRAY_BUFFER, vboID); }
	int NumVerts() { return (int)vertices.size(); }
	
	GLuint GetRenderTextureID(); // Returns texture id that will be rendered. Returns default texture is no texture is set.

	static void BindInstanceVBO() { glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); }
	static void BindTextureVBO() { glBindBuffer(GL_ARRAY_BUFFER, textureVBO); }

private:
	void UseBlendMode(BlendMode bm);

	static GLuint instanceVBO, textureVBO;

	GLuint vaoID, vboID;
	std::vector<Vertice> vertices;
	ShaderProgram *program = Shaders::defaultShader;
	BlendMode blend = BlendMode::BM_DEFAULT;
	GLenum drawMode = GL_TRIANGLES;

	AnimatedTexture* animatedTexture = NULL;
	Texture* texture = 0;
	int AT_frame = 0;
	float AT_fps, AT_timer = 0;
};

