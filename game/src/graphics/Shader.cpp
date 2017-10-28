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

ShaderProgram::Attribute::Attribute(GLuint loc, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart, bool isInstanced)
	: name{ nullptr }, index{ loc }, size{ numArgs }, type{ argType }, normalized{ (GLboolean)isNormalized }, stride{ argStride * (GLsizei)sizeofType }, start{ argStart * (GLsizei)sizeofType }, instanced{ (GLboolean)isInstanced }
{
}

void ShaderProgram::Attribute::Apply(ShaderProgram* program)
{
	if(name)
		index = glGetAttribLocation(program->m_ID, name);

	glEnableVertexAttribArray(index);

	if (type == GL_INT)
	{
		#pragma warning(suppress: 4312)
		glVertexAttribIPointer(index, size, type, stride, (void*)(start) );
	}
	else
	{
		#pragma warning(suppress: 4312)
		glVertexAttribPointer(index, size, type, normalized, stride, (void*)start);
	}

	glVertexAttribDivisor(index, instanced);
}

///
// Shader Program
///

ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& fragmentShader, std::vector<Attribute> attribs)
	: m_Attributes{attribs}
{
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexShader.GetShaderID());
	glAttachShader(m_ID, fragmentShader.GetShaderID());
	glLinkProgram(m_ID);

	GLint isLinked = 0;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) // Check for linking error
	{
		GLint logSize = 0;
		glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<char> log(logSize);
			glGetShaderInfoLog(m_ID, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not link program \n%s\n", &log[0]);
			glDeleteShader(m_ID);
		}
	}

	m_SuccessfulCompile = vertexShader.wasCompiled() && fragmentShader.wasCompiled() && (isLinked == GL_TRUE);
}

ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& geoShader, Shader& fragmentShader, std::vector<Attribute> attribs)
	: m_Attributes{ attribs }
{
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexShader.GetShaderID());
	glAttachShader(m_ID, geoShader.GetShaderID());
	glAttachShader(m_ID, fragmentShader.GetShaderID());
	glLinkProgram(m_ID);

	GLint isLinked = 0;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) // Check for linking error
	{
		GLint logSize = 0;
		glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<char> log(logSize);
			glGetShaderInfoLog(m_ID, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not link program \n%s\n", &log[0]);
			glDeleteShader(m_ID);

		}
	}

	m_SuccessfulCompile = vertexShader.wasCompiled() && geoShader.wasCompiled() && fragmentShader.wasCompiled() && (isLinked == GL_TRUE);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_ID);
}

bool ShaderProgram::wasCompiled()
{
	return m_SuccessfulCompile;
}

GLuint ShaderProgram::GetProgramID()
{ 
	return m_ID; 
}

void ShaderProgram::Use()
{ 
	glUseProgram(m_ID); 
}

void ShaderProgram::ApplyAttributes()
{
	for (Attribute& attrib : m_Attributes)
	{
		attrib.Apply(this);
	}
}

void ShaderProgram::ApplyAttributes(int start, int end)
{
	Attribute* attrib;
	for (int i = start; i < end; i++)
	{
		attrib = &m_Attributes[i];
		attrib->Apply(this);
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
	ShaderProgram* textShader;
	ShaderProgram* particleUpdateShader;
	ShaderProgram* particleRenderShader;

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
		attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(float), false, 9, 0));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(float), false, 9, 3));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 9, 7));

		// Instance Attribs
		attribs.push_back(ShaderProgram::Attribute("texBox", 4, GL_FLOAT, sizeof(float), false, 20, 0, true));
		// Model matrix
		attribs.push_back(ShaderProgram::Attribute(4, 4, GL_FLOAT, sizeof(float), false, 20, 4, true));
		attribs.push_back(ShaderProgram::Attribute(5, 4, GL_FLOAT, sizeof(float), false, 20, 8, true));
		attribs.push_back(ShaderProgram::Attribute(6, 4, GL_FLOAT, sizeof(float), false, 20, 12, true));
		attribs.push_back(ShaderProgram::Attribute(7, 4, GL_FLOAT, sizeof(float), false, 20, 16, true));

		attribs.push_back(ShaderProgram::Attribute("texLayer", 1, GL_INT, sizeof(int), false, 1, 0, true));

		defaultShader = LoadShaders(path + "shader.vertshader", path + "shader.fragshader", attribs);

		if (!defaultShader->wasCompiled())
			FailedCompile();
	}

	void LoadTextShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		// Vertex
		attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(float), false, 5, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 5, 3));

		//Character
		attribs.push_back(ShaderProgram::Attribute("charPos", 2, GL_FLOAT, sizeof(float), false, 10, 0, true));
		attribs.push_back(ShaderProgram::Attribute("texBox", 4, GL_FLOAT, sizeof(float), false, 10, 2, true));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(float), false, 10, 6, true));

		// Model matrix
//		attribs.push_back(ShaderProgram::Attribute(6, 4, GL_FLOAT, sizeof(float), false, 24, 8, true));
//		attribs.push_back(ShaderProgram::Attribute(7, 4, GL_FLOAT, sizeof(float), false, 24, 12, true));
//		attribs.push_back(ShaderProgram::Attribute(8, 4, GL_FLOAT, sizeof(float), false, 24, 16, true));
//		attribs.push_back(ShaderProgram::Attribute(9, 4, GL_FLOAT, sizeof(float), false, 24, 20, true));

		textShader = LoadShaders(path + "text.vertshader", path + "text.fragshader", attribs);

		if (!textShader->wasCompiled())
			FailedCompile();
	}

	void LoadParticleShaders()
	{
		// Particle Update Transform Feedback Shader Program
		std::string vertShaderSource = LoadFileToString( (path + "particleUpdate.vertshader").c_str() ).c_str();
		std::string geoShaderSource = LoadFileToString( (path + "particleUpdate.geoshader").c_str() ).c_str();
		Shader vs_Up = Shader(GL_VERTEX_SHADER, vertShaderSource.c_str());
		Shader gs_Up = Shader(GL_GEOMETRY_SHADER, geoShaderSource.c_str());

		GLuint updateProgram = glCreateProgram();
		glAttachShader(updateProgram, vs_Up.GetShaderID());
		glAttachShader(updateProgram, gs_Up.GetShaderID());

		const GLchar* TFVaryings[] =
		{
			"Type",
			"Position",
			"Velocity",
			"Scale",
			"Rotation",
			"Life",
			"MaxLife"
		};

		glTransformFeedbackVaryings(updateProgram, _countof(TFVaryings), TFVaryings, GL_INTERLEAVED_ATTRIBS);

		glLinkProgram(updateProgram);

		GLint isLinked = 0;
		glGetProgramiv(updateProgram, GL_LINK_STATUS, &isLinked);
		if (!vs_Up.wasCompiled() || !gs_Up.wasCompiled() || isLinked == GL_FALSE)
		{
			GLint logSize = 0;
			glGetShaderiv(updateProgram, GL_INFO_LOG_LENGTH, &logSize);
			if (logSize > 0)
			{
				std::vector<char> log(logSize);
				glGetShaderInfoLog(updateProgram, logSize, &logSize, &log[0]);
				fprintf(stderr, "Could not link program \n%s\n", &log[0]);
				glDeleteShader(updateProgram);
			}
			else
				fprintf(stderr, "Could not link program \n");
			FailedCompile();
		}

		particleUpdateShader = new ShaderProgram(updateProgram);

		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("type", 1, GL_FLOAT, sizeof(float), false, 10, 0));
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 10, 1));
		attribs.push_back(ShaderProgram::Attribute("vel", 2, GL_FLOAT, sizeof(float), false, 10, 3));
		attribs.push_back(ShaderProgram::Attribute("scale", 2, GL_FLOAT, sizeof(float), false, 10, 5));
		attribs.push_back(ShaderProgram::Attribute("rotation", 1, GL_FLOAT, sizeof(float), false, 10, 7));
		attribs.push_back(ShaderProgram::Attribute("life", 1, GL_FLOAT, sizeof(float), false, 10, 8));
		attribs.push_back(ShaderProgram::Attribute("maxLife", 1, GL_FLOAT, sizeof(float), false, 10, 9));
		particleUpdateShader->SetAttributes(attribs);

		// Particle Render Shader Program
		// Keep attribs from other shader, all that data is wanted for this shader
		
		particleRenderShader = LoadShaders(path + "particleRender.vertshader", path + "particleRender.geoshader", path + "particleRender.fragshader", attribs);
		if (!particleRenderShader->wasCompiled())
			FailedCompile();
	}

	void LoadDebugShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 9, 0, true));
		attribs.push_back(ShaderProgram::Attribute("scale", 2, GL_FLOAT, sizeof(float), false, 9, 2, true));
		attribs.push_back(ShaderProgram::Attribute("rotation", 1, GL_FLOAT, sizeof(float), false, 9, 4, true));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(float), false, 9, 5, true));

		debugShader = LoadShaders(path + "debug.vertshader", path + "debug.geoshader", path + "debug.fragshader", attribs);

		if (!debugShader->wasCompiled())
			FailedCompile();
	}

	void LoadScreenShaders()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 4, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 4, 2));

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
		LoadTextShader();
		LoadParticleShaders();
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
		delete textShader;
		delete ScreenShader::Default;
		delete ScreenShader::Blur;
		delete ScreenShader::BlurCorners;
		delete ScreenShader::ExtractBrights;
		delete ScreenShader::EdgeDetection;
		delete ScreenShader::Sharpen;
		delete ScreenShader::HDR;
		delete ScreenShader::Bloom;
		delete ScreenShader::Raindrop;
	}

};


