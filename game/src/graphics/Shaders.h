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
		- Loaded it in Init function (below) in Shader.cpp
	*/

	//Init Shaders
	void Init();

	//Unload Shaders
	void Unload();

	extern ShaderProgram* defaultShader;

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
		Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart, bool isInstanced = false);
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

	ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attributes);
	ShaderProgram(Shader& vertexShader, Shader& geoShader, Shader& fragmentShader, std::vector<Attribute> attributes);
	~ShaderProgram();

	bool wasCompiled();
	GLuint GetProgramID();
	void Use();

	template<typename T>
	void SetVariable(char* varName, T value)
	{
		Use();
		glUniform1f(glGetUniformLocation(id, varName), value);
	}

	void ShaderProgram::SetVariable(char* varName, glm::mat4 value)
	{
		Use();
		glUniformMatrix4fv(glGetUniformLocation(id, varName), 1, GL_FALSE, glm::value_ptr(value));
	}

	void ShaderProgram::SetVariable(char* varName, glm::vec2 value)
	{
		Use();
		glUniform2f(glGetUniformLocation(id, varName), value.x, value.y);
	}

	void ShaderProgram::SetVariable(char* varName, glm::vec3 value)
	{
		Use();
		glUniform3f(glGetUniformLocation(id, varName), value.x, value.y, value.z);
	}

	void ShaderProgram::SetVariable(char* varName, int value)
	{
		Use();
		glUniform1i(glGetUniformLocation(id, varName), value);
	}

	void ApplyAttributes();

private:

	GLuint id;
	std::vector<Attribute> attributes;
	bool successfulCompile = false;
};



