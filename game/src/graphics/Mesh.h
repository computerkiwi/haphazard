#pragma once

#include "Texture.h"
#include "Transform.h"
#include "Shaders.h"

#include "GL\glew.h"
#include <glm/glm.hpp>
#include <vector>

namespace Graphics
{

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

		void SetTexture(Texture& tex);
		void SetTexture(GLuint texID);
		void SetShader(ShaderProgram *shader);
		void SetBlendMode(BlendMode mode);

		std::vector<Vertice>* GetVertices();
		void CompileMesh(); // Creates mesh with provided vertices

		void Draw(glm::mat4 matrix);

	private:
		void UseBlendMode(BlendMode bm);

		GLuint vaoID, vboID;
		GLuint uniModel;
		GLenum drawMode = GL_TRIANGLES;
		std::vector<Vertice> vertices;
		GLuint texture;
		ShaderProgram *program = Shaders::defaultShader;
		BlendMode blend = BlendMode::BM_DEFAULT;
	};

}