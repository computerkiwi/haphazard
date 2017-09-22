#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"

#include "glm\glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>


///
// Mesh
///

// Statics 
GLuint Mesh::instanceVBO = 0;
static Texture* defaultTexture;

Mesh::Mesh(GLenum renderMode)
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

	if (!instanceVBO)
	{
		glGenBuffers(1, &instanceVBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	program->ApplyAttributes(3, 8); // Apply instance attributes

	glGenBuffers(1, &vboID); //Start vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	program->ApplyAttributes(0,3); // Apply non-instance attributes for vao to remember

	//Get Transform attrib locations
	uniModel = glGetUniformLocation(program->GetProgramID(), "model");
	uniTextureBox = glGetUniformLocation(program->GetProgramID(), "texBox");
}

Mesh::~Mesh()
{
	//glDeleteBuffers(1, &vboID);
	//glDeleteVertexArrays(1, &vaoID);
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

void Mesh::UseBlendMode(BlendMode bm)
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

void Mesh::Draw(glm::mat4 matrix, std::vector<float>* data)
{
	//UseBlendMode(blend);

	//program->Use();
	//glBindVertexArray(vaoID);
	//glBindBuffer(GL_ARRAY_BUFFER, vboID);

	float tex[4];
	
	if (animatedTexture)
	{
		//glBindTexture(GL_TEXTURE_2D, animatedTexture->GetID());
		glm::vec2 t = animatedTexture->GetFrameCoords(AT_frame);
		glm::vec2 s = animatedTexture->GetSpriteSize();

		//glUniform4f( uniTextureBox, t.x, t.y, t.x + s.x, t.y + s.y );
		tex[0] = t.x;
		tex[1] = t.y;
		tex[2] = t.x + s.x;
		tex[3] = t.y + s.y;
	}
	else
	{
		//if (texture)
		//	glBindTexture(GL_TEXTURE_2D, texture);
		//else
		//	glBindTexture(GL_TEXTURE_2D, defaultTexture->GetID());

		//glUniform4f(uniTextureBox, 0, 0, 1, 1);
		tex[0] = 0;
		tex[1] = 0;
		tex[2] = 1;
		tex[3] = 1;
	}

	data->push_back(tex[0]);
	data->push_back(tex[1]);
	data->push_back(tex[2]);
	data->push_back(tex[3]);

	// Load data into array
	for (int i = 0; i < 4 * 4; i++)
		data->push_back(matrix[i / 4][i % 4]);

/*	for (auto vert : vertices)
	{
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j)
			data->push_back(vert.data[j]);
		data->push_back(tex[0]);
		data->push_back(tex[1]);
		data->push_back(tex[2]);
		data->push_back(tex[3]);

		for (int i = 0; i < 4 * 4; i++)
			data->push_back(matrix[i % 4][i / 4]);
	}*/

	//numArrays += vertices.size();

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data->size(), data, GL_STATIC_DRAW);

	//glDrawArraysInstanced(drawMode, 0, (int)vertices.size(), 1);

	//data->clear();
}

void Mesh::BindTexture()
{
	if (animatedTexture)
		glBindTexture(GL_TEXTURE_2D, animatedTexture->GetID());
	else
	{
		if (texture)
			glBindTexture(GL_TEXTURE_2D, texture);
		else
			glBindTexture(GL_TEXTURE_2D, defaultTexture->GetID());
	}
}

void Mesh::SetTexture(Texture& tex) 
{ 
	texture = tex.GetID(); 
}

void Mesh::SetTexture(GLuint texID)
{
	texture = texID; 
}

void Mesh::SetShader(ShaderProgram *shader)
{
	program = shader;

	glBindVertexArray(vaoID);
	program->ApplyAttributes(); // Apply program attributes for vao to remember

	//Get Transform attrib locations
	uniModel = glGetUniformLocation(program->GetProgramID(), "model");
}

void Mesh::SetBlendMode(BlendMode b)
{
	blend = b;
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

void Mesh::UpdateAnimatedTexture(float dt)
{
	if (animatedTexture)
	{
		AT_timer += dt;
		if (AT_timer > 1.0f / AT_fps)
		{
			// Not adding by 1 prevents skipped frames in low framerates
			AT_frame += AT_timer / (1.0f / AT_fps);
			AT_timer = 0;

			if (AT_frame >= animatedTexture->GetMaxFrame())
				AT_frame = 0;
		}
	}
}
