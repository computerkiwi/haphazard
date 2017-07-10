#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

#include <glm/glm.hpp>

#include "Shaders.h"

namespace Graphics
{
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

			Screen(FX screenEffect, int width, int height, float effectIntensity = 1);

			//Check if current screen is usable
			static bool CurrentScreenIsComplete();

			GLuint ID() { return mID; }
			GLuint ColorBuffer() { return mColorBuffer; }
			GLuint DepthStencilBuffer() { return mDepthStencilBuffer; }

		private:

			void GenerateColorBuffer();
			void GenerateDepthStencilObject();

			GLuint mID;
			GLuint mColorBuffer, mDepthStencilBuffer;
			int width, height;
			float intensity;
			ShaderProgram* shader;
	};

	// Mesh to render a screen to
	class ScreenMesh
	{
		public:
			// Variables should range from 0-1, default (0,1,0,1) covers full screen
			ScreenMesh(Screen& screen, float bottom = 0, float top = 1, float left = 0, float right = 1);

			// Draws screen on mesh
			void Draw();

			// If you want to apply other effects (such as FOV change, ect) use this, make changes, then draw
			void Use() { glBindVertexArray(mVAO); }

			void SetScreen(Screen& screen) { mScreen = screen; }

			~ScreenMesh();

		private:
			GLuint mVAO, mVBO;
			Screen& mScreen;
	};

};
