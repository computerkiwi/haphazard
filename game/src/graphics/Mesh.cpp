#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"

#include "glm\glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL\SOIL.h"

///
// Mesh
///
static Graphics::Texture* defaultTexture;

Graphics::Mesh::Mesh(GLenum renderMode)
	: drawMode{ renderMode }
{
	if (!defaultTexture) // no default texture, make it
	{
		float p[] = { 
			1,1,1,1,
		};
		defaultTexture = new Texture(p, 1, 1);
	}

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(1, &vboID); //Start vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	program->ApplyAttributes(); // Apply program attributes for vao to remember

	//Get Transform attrib locations
	uniModel = glGetUniformLocation(program->GetProgramID(), "model");
}

Graphics::Mesh::~Mesh()
{
	//glDeleteBuffers(1, &vboID);
	//glDeleteVertexArrays(1, &vaoID);
}

void Graphics::Mesh::AddVertex(float x, float y, float z, float r, float g, float b, float a, float s, float t)
{
	Vertice vert = Vertice{ x, y, z, r, g, b, a, s, t };
	vertices.push_back(vert);
}

void Graphics::Mesh::AddTriangle(
	float x1, float y1, float z1, float r1, float g1, float b1, float a1, float s1, float t1,
	float x2, float y2, float z2, float r2, float g2, float b2, float a2, float s2, float t2,
	float x3, float y3, float z3, float r3, float g3, float b3, float a3, float s3, float t3)
{
	AddVertex(x1, y1, z1, r1, g1, b1, a1, s1, t1);
	AddVertex(x2, y2, z2, r2, g2, b2, a2, s2, t2);
	AddVertex(x3, y3, z3, r3, g3, b3, a3, s3, t3);
}

void Graphics::Mesh::UseBlendMode(BlendMode bm)
{
	switch (bm)
	{
	case BlendMode::BM_DISABLED:
		glBlendFunc(GL_ZERO, GL_ZERO);
		break;
	case BlendMode::BM_DEFAULT:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case BlendMode::BM_DISABLE_ALPHA:
		glBlendFunc(GL_ONE, GL_ZERO);
		break;
	case BlendMode::BM_ADDITIVE:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
}

void Graphics::Mesh::Draw(glm::mat4 matrix)
{
	UseBlendMode(blend);

	program->Use();
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
		glBindTexture(GL_TEXTURE_2D, defaultTexture->GetID());

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(matrix));

	glDrawArrays(drawMode, 0, (int)vertices.size());
}

void Graphics::Mesh::SetTexture(Texture& tex) 
{ 
	texture = tex.GetID(); 
}

void Graphics::Mesh::SetTexture(GLuint texID)
{
	texture = texID; 
}

void Graphics::Mesh::SetShader(ShaderProgram *shader)
{
	program = shader;

	glBindVertexArray(vaoID);
	program->ApplyAttributes(); // Apply program attributes for vao to remember

	//Get Transform attrib locations
	uniModel = glGetUniformLocation(program->GetProgramID(), "model");
}

void Graphics::Mesh::SetBlendMode(BlendMode b)
{
	blend = b;
}

std::vector<Graphics::Mesh::Vertice>* Graphics::Mesh::GetVertices()
{
	return &vertices;
}

void Graphics::Mesh::CompileMesh()
{
	glBindVertexArray(vaoID); // Enable vao to ensure correct mesh is compiled
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Enable vbo
	
	int s = (int)vertices.size() * Vertice::vertDataVars;
	float* verts = new float[s];
	
	int i = 0;
	for each(auto vert in vertices)
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j, ++i)
			verts[i] = vert.data[j];

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * s, verts, GL_STATIC_DRAW); //Give vbo verts
	delete [] verts;
}

///
// Texture
///
Graphics::Texture::Texture(const char* file)
{
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); // Load image
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Graphics::Texture::Texture(float *pixels, int width, int height)
{
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels); // Load image

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Graphics::Texture::~Texture()
{
	glDeleteTextures(1, &mID);
}
