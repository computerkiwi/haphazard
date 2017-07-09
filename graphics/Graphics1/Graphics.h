#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

#include <glm/glm.hpp>

#include "Shaders.h"

class Shader
{
	public:

		Shader(unsigned int shaderType, const char* source);

		Shader(unsigned int shaderType, std::string& source);

		~Shader();
		bool wasCompiled();

		GLuint GetShaderID();
	private:
		bool Compile();

		GLuint id = -1;
		bool successfulCompile = false;
};

class ShaderProgram
{
	friend class Mesh;
	friend class Camera;

	public:
		class Attribute
		{
			public:
				Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart);
				void Apply(ShaderProgram* program);

			private:
				const char* name;
				GLuint index;
				GLint size;
				GLenum type;
				GLboolean normalized;
				GLsizei stride;
				GLsizei start;
		};

		ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attributes);
		~ShaderProgram();

		bool wasCompiled();
		GLuint GetProgramID();
		void Use();

	private:

		//void AddAttribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart);
		void ApplyAttributes();

		GLuint id;
		GLuint uniView, uniProj;
		std::vector<Attribute> attributes;
		bool successfulCompile = false;

		static std::vector<ShaderProgram*> programs;
};

class Texture 
{
	public:
		Texture(const char* file);
		Texture(float *pixels, int width, int height);
		~Texture();

		GLuint GetID() { return id; }
	private:
		GLuint id;
};

class Transform
{
	public:
		Transform();
		Transform(glm::vec3 pos, glm::vec3 rotDegrees, glm::vec3 modelScale);
		glm::mat4 GetMatrix();
		void SetPosition(glm::vec3 pos);
		void SetRotation(glm::vec3 rotDegrees);
		void SetScale(glm::vec3 scl);

	private:
		glm::vec3 position; 
		glm::vec3 rotation; 
		glm::vec3 scale;
		glm::mat4 matrix;
		bool isDirty = true;
};

class Mesh
{
	public:
		struct Vertice 
		{ 
			Vertice(float x, float y, float z, float r, float g, float b, float a, float texX, float texY) 
				: data{x,y,z,r,g,b,a,texX, 1 - texY} {} //Flip y because image is loaded upside down
			
			const static int vertDataVars = 3 + 4 + 2; // Number of variables a vertice holds
			float data[vertDataVars];
		};

		Mesh(ShaderProgram* shaderProgram = Shaders::defaultShader, GLenum renderMode = GL_TRIANGLES);
		~Mesh();

		void AddVertex(float x, float y, float z, float r, float g, float b, float a, float texX, float texY);

		void AddTriangle(
			float x1, float y1, float z1, float r1, float g1, float b1, float a1, float texX1, float texY1,
			float x2, float y2, float z2, float r2, float g2, float b2, float a2, float texX2, float texY2,
			float x3, float y3, float z3, float r3, float g3, float b3, float a3, float texX3, float texY3);

		void SetTexture(Texture* tex) { texture = tex; }

		std::vector<Vertice>* GetVertices();
		void CompileMesh(); // Creates mesh with provided vertices

		void Draw();

	private:
		GLuint vaoID, vboID;
		GLuint uniModel;
		GLenum drawMode = GL_TRIANGLES;
		std::vector<Vertice> vertices;
		Texture* texture;
		ShaderProgram program;

	public:
		Transform transform;
};

class Camera
{
	public:
		void SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector);
		void SetProjection(float FOV, float aspectRatio, float near, float far);

		void SetPosition(glm::vec3 pos) { mPosition = pos; ApplyCameraMatrices(); }
		void SetTarget(glm::vec3 pos) { mCenter = pos; ApplyCameraMatrices(); }
		void SetUp(glm::vec3 up) { mUp = up; ApplyCameraMatrices(); }

		void SetFOV(float fovDegrees) { mFOV = fovDegrees; ApplyCameraMatrices(); }
		void SetAspectRatio(float ar) { mAspectRatio = ar; ApplyCameraMatrices(); }
		void SetNearPlane(float near) { mNear = near; ApplyCameraMatrices(); }
		void SetFarPlane(float far) { mFar = far; ApplyCameraMatrices(); }

		void MoveCamera(glm::vec3 pos) 
		{
			mCenter += pos - mPosition;
			mPosition = pos;
			ApplyCameraMatrices();
		}

		glm::vec3 GetPosition() { return mPosition; }
		float GetFOV() { return mFOV; }

		void Orbit(float degrees, glm::vec3 axis); // Rotates around target
		void OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis); // Rotates around center
		
		void Use() { ApplyCameraMatrices(); }

	private:
		void ApplyCameraMatrices();

		//View matrix
		glm::vec3 mPosition = glm::vec3(0,0,5.0f);
		glm::vec3 mCenter = glm::vec3(0,0,0);
		glm::vec3 mUp = glm::vec3(0,0,1);

		//Projection matrix
		float mFOV = 45.0f;
		float mAspectRatio;
		float mNear = 1.0f;
		float mFar = 10.0f;
};

class Screen
{
	public:
		enum FX 
		{
			DEFAULT,
			EDGE_DETECTION,
			SHARPEN,
			BLUR,
			BLUR_CORNERS,
			BLUR_FOCUS,
		};

		Screen(FX screenEffect, int width, int height, float effectIntensity = 1)
			: width{ width }, height{ height }, intensity{ effectIntensity } 
		{
			glGenFramebuffers(1, &mID);
			glBindFramebuffer(GL_FRAMEBUFFER, mID);

			switch (screenEffect)
			{
				default:
					shader = Shaders::defaultScreenShader;
				case EDGE_DETECTION:

					break;
			}
		}

		bool IsComplete()
		{
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				return false;
			return true;
		}

	private:

		void GenerateColorBuffer()
		{
			// create a color attachment texture
			unsigned int textureColorbuffer;
			glGenTextures(1, &textureColorbuffer);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		}

		void GenerateDepthStencilObject()
		{
			// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
			glGenRenderbuffers(1, &mDepthStencilBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer); // now actually attach it
		}

		void GenerateDepthStencilTexture()
		{
		}

		GLuint mID;
		GLuint mColorBuffer, mDepthStencilBuffer;
		int width, height;
		int intensity;
		ShaderProgram* shader;
};