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

	uniView = glGetUniformLocation(id, "view"); // For non-screen shaders that support camera input
	uniProj = glGetUniformLocation(id, "proj"); // ^^^

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
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::Default;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::HDR;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::Blur;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::BlurCorners;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::EdgeDetection;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::Sharpen;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::ExtractBrights;
Graphics::ShaderProgram* Graphics::Shaders::ScreenShader::Bloom;

void LoadDefaultShader()
{
	std::vector<Graphics::ShaderProgram::Attribute> attribs;
	attribs.push_back(Graphics::ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0));
	attribs.push_back(Graphics::ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3));
	attribs.push_back(Graphics::ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7));

	Graphics::Shaders::defaultShader = LoadShaders("shader.vertshader", "shader.fragshader", attribs);

	if (!Graphics::Shaders::defaultShader->wasCompiled())
		FailedCompile();
}

void LoadScreenShaders()
{
	std::vector<Graphics::ShaderProgram::Attribute> attribs;
	attribs.push_back(Graphics::ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 4, 0));
	attribs.push_back(Graphics::ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 4, 2));

	Graphics::Shaders::ScreenShader::Default = LoadShaders("screenDefault.vertshader", "screenDefault.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::Default->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::HDR = LoadShaders("screenDefault.vertshader", "screenHDR.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::HDR->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::Blur = LoadShaders("screenDefault.vertshader", "screenBlur.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::Blur->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::BlurCorners = LoadShaders("screenDefault.vertshader", "screenBlurCorners.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::BlurCorners->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::EdgeDetection = LoadShaders("screenDefault.vertshader", "screenEdgeDect.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::EdgeDetection->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::Sharpen= LoadShaders("screenDefault.vertshader", "screenSharpen.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::Sharpen->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::ExtractBrights = LoadShaders("screenDefault.vertshader", "screenExtractBrights.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::ExtractBrights->wasCompiled())
		FailedCompile();

	Graphics::Shaders::ScreenShader::Bloom = LoadShaders("screenDefault.vertshader", "screenBloom.fragshader", attribs);
	if (!Graphics::Shaders::ScreenShader::Bloom->wasCompiled())
		FailedCompile();
}

void LoadDefaultShaderUniforms()
{
	Graphics::Shaders::ScreenShader::Blur->SetVariable("Intensity", 1.0f);
	Graphics::Shaders::ScreenShader::BlurCorners->SetVariable("Intensity", 3.0f);
	Graphics::Shaders::ScreenShader::EdgeDetection->SetVariable("Intensity", 1.0f);
	Graphics::Shaders::ScreenShader::Sharpen->SetVariable("Intensity", 1.0f);
	Graphics::Shaders::ScreenShader::HDR->SetVariable("Exposure", 1.0f);

	Graphics::Shaders::ScreenShader::Bloom->SetVariable("screenTexture", 0);
	Graphics::Shaders::ScreenShader::Bloom->SetVariable("brights", 1);
	//Graphics::Shaders::ScreenShader::Bloom->Use();
	//glUniform1i(glGetUniformLocation(Graphics::Shaders::ScreenShader::Bloom->GetProgramID(), "screenTexture"), 0);
	//glUniform1i(glGetUniformLocation(Graphics::Shaders::ScreenShader::Bloom->GetProgramID(), "brights"), 1);
}


// Creates all basic shaders
void Graphics::Shaders::Init()
{
	LoadDefaultShader();
	LoadScreenShaders();

	LoadDefaultShaderUniforms();
}

// Frees all basic shaders
void Graphics::Shaders::Unload()
{
	delete defaultShader;
	delete ScreenShader::Default;
	delete ScreenShader::Blur;
	delete ScreenShader::BlurCorners;
	delete ScreenShader::ExtractBrights;
	delete ScreenShader::EdgeDetection;
	delete ScreenShader::Sharpen;
}
