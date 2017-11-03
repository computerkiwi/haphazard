/*
FILE: Shaders.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class ShaderProgram;

namespace Shaders
{
	/*
	To add new shaders:
		- Add new variable in appropriate location
		- Load it in Init function in Shader.cpp (use existing functions as template)
	*/

	//Init Shaders
	void Init();

	//Unload Shaders
	void Unload();

	extern ShaderProgram* spriteShader;
	extern ShaderProgram* textShader;
	extern ShaderProgram* particleUpdateShader;
	extern ShaderProgram* particleRenderShader;
	extern ShaderProgram* backgroundShader;

	extern ShaderProgram* debugShader;

	namespace ScreenShader
	{
		extern ShaderProgram* Default;
		extern ShaderProgram* HDR;
		extern ShaderProgram* EdgeDetection;
		extern ShaderProgram* Sharpen;
		extern ShaderProgram* Blur;
		extern ShaderProgram* BlurCorners;
		extern ShaderProgram* ExtractBrights;
		extern ShaderProgram* Bloom;

		extern ShaderProgram* Raindrop;
	}


};

///
// Classes
///

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

	GLuint m_ID = -1;
	bool m_SuccessfulCompile = false;
};



class ShaderProgram
{
	friend class Mesh;
	friend class Camera;

public:
	class Attribute;
	
	// Constructors

	ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attributes);
	ShaderProgram(Shader& vertexShader, Shader& geoShader, Shader& fragmentShader, std::vector<Attribute> attributes);
	
	/*
	Constructs shader from shader ID alone. 
	Warning: Does not check or hold compilation status of program or shaders, it will always return false
	*/
	explicit ShaderProgram(GLuint id) : m_ID{id} {} 
	
	~ShaderProgram();

	bool wasCompiled();
	GLuint GetProgramID();
	void Use(); // Binds shader program

	// Sets attributes to new list. Only use if you know what youre doing, and you would know you shouldn't be doing this.
	void SetAttributes(std::vector<Attribute> attributes) { m_Attributes = attributes; }

	// Applies all vertex attributes for shader. Only use if you know what youre doing.
	void ApplyAttributes();
	// Applies specific attributes. Only use if you know what youre doing.
	void ApplyAttributes(int start, int end); 

	/* 
	Set variable sets uniforms of a shader to a specific value.
	Be specific in the types you send or else the variable may not be set correctly which could
	result in an OpenGL error.
	*/
	template<typename T>
	void SetVariable(char* varName, T value)
	{
		Use();
		glUniform1f(glGetUniformLocation(m_ID, varName), value);
	}

private:

	GLuint m_ID;
	std::vector<Attribute> m_Attributes;
	bool m_SuccessfulCompile = false;

public: 
	// SetVariable Specifications

	void ShaderProgram::SetVariable(char* varName, glm::mat4 value)
	{
		Use();
		glUniformMatrix4fv(glGetUniformLocation(m_ID, varName), 1, GL_FALSE, glm::value_ptr(value));
	}

	void ShaderProgram::SetVariable(char* varName, glm::vec2 value)
	{
		Use();
		glUniform2f(glGetUniformLocation(m_ID, varName), value.x, value.y);
	}

	void ShaderProgram::SetVariable(char* varName, glm::vec3 value)
	{
		Use();
		glUniform3f(glGetUniformLocation(m_ID, varName), value.x, value.y, value.z);
	}

	void ShaderProgram::SetVariable(char* varName, glm::vec4 value)
	{
		Use();
		glUniform4f(glGetUniformLocation(m_ID, varName), value.x, value.y, value.z, value.w);
	}

	void ShaderProgram::SetVariable(char* varName, int value)
	{
		Use();
		glUniform1i(glGetUniformLocation(m_ID, varName), value);
	}

	void ShaderProgram::SetVariable(char* varName, GLuint value)
	{
		Use();
		glUniform1ui(glGetUniformLocation(m_ID, varName), value);
	}

	///
	// Attribute Class
	///

	// Vertex attribute class, holds all necessary data for glVertexAttribPointer function call
	class Attribute
	{
	public:
		Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart, bool isInstanced = false);
		Attribute(GLuint location, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart, bool isInstanced = false);
		void Apply(ShaderProgram* program);

	private:
		const char* name;
		GLuint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLboolean instanced;
		GLsizei stride;
		GLsizei start;
	};
};
