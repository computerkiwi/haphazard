#include <algorithm>
#include <iostream>
#include <fstream>

#include "Graphics.h"
#include "Shaders.h"

///
// Shader
///

Shader::Shader(unsigned int shaderType, const char* source)
{
	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &source, NULL);
	successfulCompile = Compile();
}

Shader::Shader(unsigned int shaderType, std::string& source)
{
	const GLchar* c_source = source.c_str();
	glShaderSource(id, 1, &c_source, NULL);
	successfulCompile = Compile();
}

Shader::~Shader() 
{ 
	glDeleteShader(id); 
}

bool Shader::wasCompiled() 
{ 
	return successfulCompile; 
}

GLuint Shader::GetShaderID() { 
	return id; 
}

bool Shader::Compile()
{
	glCompileShader(id);

	//Check compile success
	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<GLchar> log(logSize);
			glGetShaderInfoLog(id, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not compile shader \n%s\n", &log[0]);
			glDeleteShader(id);
		}
	}
	return success;
}

///
// Attribute
///

ShaderProgram::Attribute::Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart)
	: name{ name }, size{ numArgs }, type{ argType }, normalized{ (unsigned char)isNormalized }, stride{ argStride * (GLsizei)sizeofType }, start{ argStart * (GLsizei)sizeofType }
{
}

void ShaderProgram::Attribute::Apply(ShaderProgram* program)
{
	index = glGetAttribLocation(program->id, name);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(start));
}

///
// Shader Program
///

ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attribs)
	: attributes{attribs}
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader.GetShaderID());
	glAttachShader(id, fragmentShader.GetShaderID());
	glLinkProgram(id);

	successfulCompile = vertexShader.wasCompiled() && fragmentShader.wasCompiled();

	uniView = glGetUniformLocation(id, "view");
	uniProj = glGetUniformLocation(id, "proj");

	programs.push_back(this);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

bool ShaderProgram::wasCompiled()
{
	return successfulCompile;
}

GLuint ShaderProgram::GetProgramID() 
{ 
	return id; 
}

void ShaderProgram::Use() 
{ 
	glUseProgram(id); 
}

void ShaderProgram::ApplyAttributes()
{
	for each (Attribute attrib in attributes)
	{
		attrib.Apply(this);
	}
}

///
// Shader Helper Functions
///

static std::string LoadFileToString(const char* filepath)
{
	std::string fileData = "";
	std::ifstream stream(filepath, std::ios::in);

	if (stream.is_open())
	{
		std::string line = "";
		while (getline(stream, line))
		{
			fileData += "\n" + line;
		}

		stream.close();
	}

	return fileData;
}

static ShaderProgram* LoadShaders(const char* vertShaderPath, const char* fragShaderPath, std::vector<ShaderProgram::Attribute>& attribs)
{
	std::string vertShaderSource = LoadFileToString(vertShaderPath).c_str();
	std::string fragShaderSource = LoadFileToString(fragShaderPath).c_str();

	return new ShaderProgram(Shader(GL_VERTEX_SHADER, vertShaderSource.c_str()), Shader(GL_FRAGMENT_SHADER, fragShaderSource.c_str()), attribs);
}

static void FailedCompile()
{
	glfwTerminate();
	fprintf(stderr, "Could not compile shader");
	int a;
	std::cin >> a;
	exit(1);
}

///
// Load Shaders
///

//Shader Declaration
ShaderProgram* Shaders::defaultShader;
ShaderProgram* Shaders::defaultScreenShader;

static void LoadDefaultShader()
{
	std::vector<ShaderProgram::Attribute> attribs;
	attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0));
	attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3));
	attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7));

	Shaders::defaultShader = LoadShaders("shader.vertshader", "shader.fragshader", attribs);

	if (!Shaders::defaultShader->wasCompiled())
		FailedCompile();
}

static void LoadDefaultScreenShader()
{
	std::vector<ShaderProgram::Attribute> attribs;
	attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 5, 0));
	attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 5, 3));

	Shaders::defaultScreenShader = LoadShaders("screen.vertshader", "screen.fragshader", attribs);

	if (!Shaders::defaultScreenShader->wasCompiled())
		FailedCompile();
}

// Creates all basic shaders
void Shaders::Init()
{
	LoadDefaultShader();
	LoadDefaultScreenShader();
}

// Frees all basic shaders
void Shaders::Unload()
{
	delete defaultShader;
}