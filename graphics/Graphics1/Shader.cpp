#include <algorithm>
#include <iostream>
#include <fstream>

#include "Graphics.h"
#include "Shaders.h"

///
// Shader
///

Graphics::Shader::Shader(unsigned int shaderType, const char* source)
{
	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &source, NULL);
	successfulCompile = Compile();
}

Graphics::Shader::Shader(unsigned int shaderType, std::string& source)
{
	const GLchar* c_source = source.c_str();
	glShaderSource(id, 1, &c_source, NULL);
	successfulCompile = Compile();
}

Graphics::Shader::~Shader()
{ 
	glDeleteShader(id); 
}

bool Graphics::Shader::wasCompiled()
{ 
	return successfulCompile; 
}

GLuint Graphics::Shader::GetShaderID() {
	return id; 
}

bool Graphics::Shader::Compile()
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

Graphics::ShaderProgram::Attribute::Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart)
	: name{ name }, size{ numArgs }, type{ argType }, normalized{ (unsigned char)isNormalized }, stride{ argStride * (GLsizei)sizeofType }, start{ argStart * (GLsizei)sizeofType }
{
}

void Graphics::ShaderProgram::Attribute::Apply(ShaderProgram* program)
{
	index = glGetAttribLocation(program->id, name);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(start));
}

///
// Shader Program
///

Graphics::ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attribs)
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

Graphics::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(id);
}

bool Graphics::ShaderProgram::wasCompiled()
{
	return successfulCompile;
}

GLuint Graphics::ShaderProgram::GetProgramID()
{ 
	return id; 
}

void Graphics::ShaderProgram::Use()
{ 
	glUseProgram(id); 
}

void Graphics::ShaderProgram::ApplyAttributes()
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

static Graphics::ShaderProgram* LoadShaders(const char* vertShaderPath, const char* fragShaderPath, std::vector<Graphics::ShaderProgram::Attribute>& attribs)
{
	std::string vertShaderSource = LoadFileToString(vertShaderPath).c_str();
	std::string fragShaderSource = LoadFileToString(fragShaderPath).c_str();

	return new Graphics::ShaderProgram(Graphics::Shader(GL_VERTEX_SHADER, vertShaderSource.c_str()), Graphics::Shader(GL_FRAGMENT_SHADER, fragShaderSource.c_str()), attribs);
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
Graphics::ShaderProgram* Graphics::Shaders::defaultShader;
Graphics::ShaderProgram* Graphics::Shaders::defaultScreenShader;

static void LoadDefaultShader()
{
	std::vector<Graphics::ShaderProgram::Attribute> attribs;
	attribs.push_back(Graphics::ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0));
	attribs.push_back(Graphics::ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3));
	attribs.push_back(Graphics::ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7));

	Graphics::Shaders::defaultShader = LoadShaders("shader.vertshader", "shader.fragshader", attribs);

	if (!Graphics::Shaders::defaultShader->wasCompiled())
		FailedCompile();
}

static void LoadDefaultScreenShader()
{
	std::vector<Graphics::ShaderProgram::Attribute> attribs;
	attribs.push_back(Graphics::ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 5, 0));
	attribs.push_back(Graphics::ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 5, 3));

	Graphics::Shaders::defaultScreenShader = LoadShaders("screen.vertshader", "screen.fragshader", attribs);

	if (!Graphics::Shaders::defaultScreenShader->wasCompiled())
		FailedCompile();
}

// Creates all basic shaders
void Graphics::Shaders::Init()
{
	LoadDefaultShader();
	LoadDefaultScreenShader();
}

// Frees all basic shaders
void Graphics::Shaders::Unload()
{
	delete defaultShader;
}