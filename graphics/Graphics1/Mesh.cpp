#include "Graphics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

///
// Mesh
///
static Texture* defaultTexture;

Mesh::Mesh(ShaderProgram* shaderProgram, GLenum renderMode) 
	: program{ *shaderProgram }, drawMode{ renderMode }
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

	program.ApplyAttributes(); // Apply program attributes for vao to remember

	//Get Transform attrib locations
	glGetUniformLocation(shaderProgram->GetProgramID(), "model");
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vboID);
	glDeleteVertexArrays(1, &vaoID);
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

void Mesh::Draw()
{
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture->GetID());
	else
		glBindTexture(GL_TEXTURE_2D, defaultTexture->GetID());

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(transform.GetMatrix()));

	glDrawArrays(drawMode, 0, (int)vertices.size());
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
	for each(auto vert in vertices)
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j, ++i)
			verts[i] = vert.data[j];

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * s, verts, GL_STATIC_DRAW); //Give vbo verts
	delete [] verts;
}

///
// Texture
///
Texture::Texture(const char* file)
{
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); // Load image
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::Texture(float *pixels, int width, int height)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels); // Load image

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

///
// Transform
///

Transform::Transform()
	: scale{ glm::vec3(1,1,1) }
{ }

Transform::Transform(glm::vec3 pos, glm::vec3 rotDegrees, glm::vec3 modelScale)
	: position{ pos }, rotation{ rotDegrees }, scale{ modelScale }
{ }

glm::mat4 Transform::GetMatrix()
{
	if (isDirty)
	{
		matrix = glm::mat4();
		matrix = glm::translate(matrix, position);
		matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, scale);

		isDirty = false;
	}
	return matrix;
}

void Transform::SetPosition(glm::vec3 pos)
{
	position = pos;
	isDirty = true;
}

void Transform::SetRotation(glm::vec3 rotDegrees)
{
	rotation = rotDegrees;
	isDirty = true;
}

void Transform::SetScale(glm::vec3 scl)
{
	scale = scl;
	isDirty = true;
}

///
// Camera
///

std::vector<ShaderProgram*> ShaderProgram::programs;

void Camera::SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector)
{
	mPosition = pos;
	mCenter = target;
	mUp = upVector;
	ApplyCameraMatrices();
}

void Camera::SetProjection(float FOV, float aspectRatio, float near, float far)
{
	mFOV = FOV;
	mAspectRatio = aspectRatio;
	mNear = near;
	mFar = far;
	ApplyCameraMatrices();
}

void Camera::ApplyCameraMatrices()
{
	glm::mat4 view = glm::lookAt(mPosition,	mCenter, mUp);
	glm::mat4 proj = glm::perspective(glm::radians(mFOV), mAspectRatio,	mNear, mFar);
	
	for each(ShaderProgram* program in ShaderProgram::programs)
	{
		glUniformMatrix4fv(program->uniView, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(program->uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	}
}

void Camera::Orbit(float degrees, glm::vec3 axis) // Rotates around target
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, mCenter);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -mCenter);

	glm::vec3 forward = mPosition - mCenter;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		mPosition = matrix * glm::vec4(mPosition, 1);
		mUp = matrix * glm::vec4(mUp, 1);
	}
	else
		mPosition = matrix * glm::vec4(mPosition, 1);

	ApplyCameraMatrices();
}

void Camera::OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis) // Rotates around center
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, center);
	matrix = glm::rotate(matrix, glm::radians(degrees), axis);
	matrix = glm::translate(matrix, -center);

	glm::vec3 forward = mPosition - mCenter;

	if ((axis.x && forward.x) || (axis.y && forward.y) || (axis.z && forward.z)) // If axis contains forward
	{
		mPosition = matrix * glm::vec4(mPosition, 1);
		mUp = matrix * glm::vec4(mUp, 1);
	}
	else
		mPosition = matrix * glm::vec4(mPosition, 1);

	ApplyCameraMatrices();
}
