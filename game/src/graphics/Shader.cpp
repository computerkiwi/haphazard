#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Shaders.h"
#include "GLFW\glfw3.h"
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
	const char* c_source = source.c_str();
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
			std::vector<char> log(logSize);
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

ShaderProgram::Attribute::Attribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart, bool isInstanced)
	: name{ name }, size{ numArgs }, type{ argType }, normalized{ (GLboolean)isNormalized }, stride{ argStride * (GLsizei)sizeofType }, start{ argStart * (GLsizei)sizeofType }, instanced{ (GLboolean)isInstanced }
{
}

void ShaderProgram::Attribute::Apply(ShaderProgram* program)
{
	index = glGetAttribLocation(program->id, name);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(start));
	glVertexAttribDivisor(index, instanced);
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
}

ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& geoShader, Shader& fragmentShader, std::vector<Attribute> attribs)
	: attributes{ attribs }
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader.GetShaderID());
	glAttachShader(id, geoShader.GetShaderID());
	glAttachShader(id, fragmentShader.GetShaderID());
	glLinkProgram(id);

	successfulCompile = vertexShader.wasCompiled() && geoShader.wasCompiled() && fragmentShader.wasCompiled();
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
	for (Attribute& attrib : attributes)
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

static ShaderProgram* LoadShaders(const char* vertShaderPath, const char* geoShaderPath, const char* fragShaderPath, std::vector<ShaderProgram::Attribute>& attribs)
{
	std::string vertShaderSource = LoadFileToString(vertShaderPath).c_str();
	std::string geoShaderSource = LoadFileToString(geoShaderPath).c_str();
	std::string fragShaderSource = LoadFileToString(fragShaderPath).c_str();

	return new ShaderProgram(Shader(GL_VERTEX_SHADER, vertShaderSource.c_str()), Shader(GL_GEOMETRY_SHADER, geoShaderSource.c_str()), Shader(GL_FRAGMENT_SHADER, fragShaderSource.c_str()), attribs);
}

static ShaderProgram* LoadShaders(std::string vertShaderPath, std::string fragShaderPath, std::vector<ShaderProgram::Attribute>& attribs)
{
	return LoadShaders(vertShaderPath.c_str(), fragShaderPath.c_str(), attribs);
}

static ShaderProgram* LoadShaders(std::string vertShaderPath, std::string geoShaderPath, std::string fragShaderPath, std::vector<ShaderProgram::Attribute>& attribs)
{
	return LoadShaders(vertShaderPath.c_str(), geoShaderPath.c_str(), fragShaderPath.c_str(), attribs);
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

static const std::string path = "shaders/";

namespace Shaders
{
	//Shader Declaration
	ShaderProgram* defaultShader;
	ShaderProgram* debugShader;

	ShaderProgram* ScreenShader::Raindrop;

	ShaderProgram* ScreenShader::Default;
	ShaderProgram* ScreenShader::HDR;
	ShaderProgram* ScreenShader::Blur;
	ShaderProgram* ScreenShader::BlurCorners;
	ShaderProgram* ScreenShader::EdgeDetection;
	ShaderProgram* ScreenShader::Sharpen;
	ShaderProgram* ScreenShader::ExtractBrights;
	ShaderProgram* ScreenShader::Bloom;

	void LoadDefaultShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7));

		defaultShader = LoadShaders(path + "shader.vertshader", path + "shader.fragshader", attribs);

		if (!defaultShader->wasCompiled())
			FailedCompile();
	}

	void LoadDebugShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0, true));
		attribs.push_back(ShaderProgram::Attribute("scale", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 2, true));
		attribs.push_back(ShaderProgram::Attribute("rotation", 1, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 4, true));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 5, true));

		debugShader = LoadShaders(path + "debug.vertshader", path + "debug.geoshader", path + "debug.fragshader", attribs);

		if (!debugShader->wasCompiled())
			FailedCompile();
	}

	void LoadScreenShaders()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 4, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 4, 2));

		ScreenShader::Default = LoadShaders(path + "screenDefault.vertshader", path + "screenDefault.fragshader", attribs);
		if (!ScreenShader::Default->wasCompiled())
			FailedCompile();

		ScreenShader::HDR = LoadShaders(path + "screenDefault.vertshader", path + "screenHDR.fragshader", attribs);
		if (!ScreenShader::HDR->wasCompiled())
			FailedCompile();

		ScreenShader::Blur = LoadShaders(path + "screenDefault.vertshader", path + "screenBlur.fragshader", attribs);
		if (!ScreenShader::Blur->wasCompiled())
			FailedCompile();

		ScreenShader::BlurCorners = LoadShaders(path + "screenDefault.vertshader", path + "screenBlurCorners.fragshader", attribs);
		if (!ScreenShader::BlurCorners->wasCompiled())
			FailedCompile();

		ScreenShader::EdgeDetection = LoadShaders(path + "screenDefault.vertshader", path + "screenEdgeDect.fragshader", attribs);
		if (!ScreenShader::EdgeDetection->wasCompiled())
			FailedCompile();

		ScreenShader::Sharpen= LoadShaders(path + "screenDefault.vertshader", path + "screenSharpen.fragshader", attribs);
		if (!ScreenShader::Sharpen->wasCompiled())
			FailedCompile();

		ScreenShader::ExtractBrights = LoadShaders(path + "screenDefault.vertshader", path + "screenExtractBrights.fragshader", attribs);
		if (!ScreenShader::ExtractBrights->wasCompiled())
			FailedCompile();

		ScreenShader::Bloom = LoadShaders(path + "screenDefault.vertshader", path + "screenBloom.fragshader", attribs);
		if (!ScreenShader::Bloom->wasCompiled())
			FailedCompile();

		ScreenShader::Raindrop = LoadShaders(path + "raindrop.vertshader", path + "raindrop.fragshader", attribs);
		if (!ScreenShader::Raindrop->wasCompiled())
			FailedCompile();
	}

	void LoadDefaultShaderUniforms()
	{
		ScreenShader::Blur->SetVariable("Intensity", 1.0f);
		ScreenShader::BlurCorners->SetVariable("Intensity", 3.0f);
		ScreenShader::EdgeDetection->SetVariable("Intensity", 1.0f);
		ScreenShader::Sharpen->SetVariable("Intensity", 1.0f);
		ScreenShader::HDR->SetVariable("Exposure", 1.0f);

		ScreenShader::Bloom->SetVariable("screenTexture", 0);
		ScreenShader::Bloom->SetVariable("brights", 1);

		ScreenShader::Raindrop->SetVariable("raindropTexture", 0);
		ScreenShader::Raindrop->SetVariable("screenTexture", 1);
	}

	void LoadUniformBlockBindings()
	{
		//Bind all shaders with Matrices uniform (for camera matrices) to 0
		GLuint index = glGetUniformBlockIndex(defaultShader->GetProgramID(), "Matrices");
		glUniformBlockBinding(defaultShader->GetProgramID(), index, 0);
	}

	// Creates all basic shaders
	void Init()
	{
		LoadDefaultShader();
		LoadDebugShader();
		LoadScreenShaders();

		LoadDefaultShaderUniforms();
		LoadUniformBlockBindings();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
	}

	// Frees all basic shaders
	void Unload()
	{
		delete defaultShader;
		delete debugShader;
		delete ScreenShader::Default;
		delete ScreenShader::Blur;
		delete ScreenShader::BlurCorners;
		delete ScreenShader::ExtractBrights;
		delete ScreenShader::EdgeDetection;
		delete ScreenShader::Sharpen;
		delete ScreenShader::HDR;
		delete ScreenShader::Bloom;
	}

};


