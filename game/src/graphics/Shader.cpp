/*
FILE: Shader.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
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
	m_ID = glCreateShader(shaderType);
	glShaderSource(m_ID, 1, &source, NULL);
	m_SuccessfulCompile = Compile();
}

Shader::Shader(unsigned int shaderType, std::string& source)
{
	const char* c_source = source.c_str();
	glShaderSource(m_ID, 1, &c_source, NULL);
	m_SuccessfulCompile = Compile();
}

Shader::~Shader()
{ 
	glDeleteShader(m_ID); 
}

bool Shader::wasCompiled()
{ 
	return m_SuccessfulCompile; 
}

GLuint Shader::GetShaderID() {
	return m_ID; 
}

bool Shader::Compile()
{
	glCompileShader(m_ID);

	//Check compile success
	GLint success = 0;
	glGetShaderiv(m_ID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<char> log(logSize);
			glGetShaderInfoLog(m_ID, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not compile shader \n%s\n", &log[0]);
			glDeleteShader(m_ID);
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

void ShaderProgram::Attribute::SetShader(ShaderProgram* program)
{
	if(name)
		index = glGetAttribLocation(program->m_ID, name);
}

void ShaderProgram::Attribute::Apply()
{
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
		glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<char> log(logSize);
			glGetProgramInfoLog(m_ID, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not link program \n%s\n", &log[0]);
			glDeleteProgram(m_ID);
		}
		else
			fprintf(stderr, "Could not link program, no log available\n");
	}

	m_SuccessfulCompile = vertexShader.wasCompiled() && fragmentShader.wasCompiled() && (isLinked == GL_TRUE);

	for (auto& a : m_Attributes)
		a.SetShader(this);
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
		glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<char> log(logSize);
			glGetProgramInfoLog(m_ID, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not link program \n%s\n", &log[0]);
			glDeleteProgram(m_ID);
		}
		else
			fprintf(stderr, "Could not link program, no log available\n");
	}

	m_SuccessfulCompile = vertexShader.wasCompiled() && geoShader.wasCompiled() && fragmentShader.wasCompiled() && (isLinked == GL_TRUE);

	for (auto& a : m_Attributes)
		a.SetShader(this);
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
		attrib.Apply();
	}
}

void ShaderProgram::ApplyAttributes(int start, int end)
{
	Attribute* attrib;
	for (int i = start; i < end; i++)
	{
		attrib = &m_Attributes[i];
		attrib->Apply();
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
	ShaderProgram* spriteShader;
	ShaderProgram* textShader;
	ShaderProgram* particleUpdateShader;
	ShaderProgram* particleRenderShader;
	ShaderProgram* backgroundShader;

	ShaderProgram* debugShader;
	ShaderProgram* editorSpriteShader;

	ShaderProgram* ScreenShader::Raindrop;

	ShaderProgram* ScreenShader::Default;
	ShaderProgram* ScreenShader::HDR;
	ShaderProgram* ScreenShader::Blur;
	ShaderProgram* ScreenShader::BlurCorners;
	ShaderProgram* ScreenShader::EdgeDetection;
	ShaderProgram* ScreenShader::Sharpen;
	ShaderProgram* ScreenShader::ExtractBrights;
	ShaderProgram* ScreenShader::Bloom;
	ShaderProgram* ScreenShader::Dropshadow;

	void LoadSpriteShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(float), false, 9, 0));
		attribs.push_back(ShaderProgram::Attribute("vcolor", 4, GL_FLOAT, sizeof(float), false, 9, 3));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 9, 7));

		// Instance Attribs
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(float), false, 26, 0, true));
		attribs.push_back(ShaderProgram::Attribute("texBox", 4, GL_FLOAT, sizeof(float), false, 26, 4, true));
		// Model matrix
		attribs.push_back(ShaderProgram::Attribute(5, 4, GL_FLOAT, sizeof(float), false, 26, 8, true));
		attribs.push_back(ShaderProgram::Attribute(6, 4, GL_FLOAT, sizeof(float), false, 26, 12, true));
		attribs.push_back(ShaderProgram::Attribute(7, 4, GL_FLOAT, sizeof(float), false, 26, 16, true));
		attribs.push_back(ShaderProgram::Attribute(8, 4, GL_FLOAT, sizeof(float), false, 26, 20, true));
		attribs.push_back(ShaderProgram::Attribute("texScale", 2, GL_FLOAT, sizeof(float), false, 26, 24, true));

		attribs.push_back(ShaderProgram::Attribute("texLayer", 1, GL_INT, sizeof(int), false, 1, 0, true));

		spriteShader = LoadShaders(path + "sprite.vert", path + "sprite.frag", attribs);

		if (!spriteShader->wasCompiled())
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

		textShader = LoadShaders(path + "text.vert", path + "text.frag", attribs);

		if (!textShader->wasCompiled())
			FailedCompile();
	}

	void LoadBackgroundShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 4, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 4, 2));

		backgroundShader = LoadShaders(path + "background.vert", path + "background.frag", attribs);
		if (!backgroundShader->wasCompiled())
			FailedCompile();
	}

	void LoadParticleShaders()
	{
		// Particle Update Transform Feedback Shader Program
		std::string vertShaderSource = LoadFileToString( (path + "particleUpdate.vert").c_str() ).c_str();
		std::string geoShaderSource = LoadFileToString( (path + "particleUpdate.geo").c_str() ).c_str();
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
			"MaxLife",
			"Frame",
			"Seed"
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
		attribs.push_back(ShaderProgram::Attribute("type", 1, GL_FLOAT, sizeof(float), false, 12, 0));
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 12, 1));
		attribs.push_back(ShaderProgram::Attribute("vel", 2, GL_FLOAT, sizeof(float), false, 12, 3));
		attribs.push_back(ShaderProgram::Attribute("scale", 2, GL_FLOAT, sizeof(float), false, 12, 5));
		attribs.push_back(ShaderProgram::Attribute("rotation", 1, GL_FLOAT, sizeof(float), false, 12, 7));
		attribs.push_back(ShaderProgram::Attribute("life", 1, GL_FLOAT, sizeof(float), false, 12, 8));
		attribs.push_back(ShaderProgram::Attribute("maxLife", 1, GL_FLOAT, sizeof(float), false, 12, 9));
		attribs.push_back(ShaderProgram::Attribute("frame", 1, GL_FLOAT, sizeof(float), false, 12, 10));
		attribs.push_back(ShaderProgram::Attribute("seed", 1, GL_FLOAT, sizeof(float), false, 12, 11));
		particleUpdateShader->SetAttributes(attribs);

		// Particle Render Shader Program
		// Keep attribs from other shader, all that data is wanted for this shader
		
		particleRenderShader = LoadShaders(path + "particleRender.vert", path + "particleRender.geo", path + "particleRender.frag", attribs);
		if (!particleRenderShader->wasCompiled())
			FailedCompile();
	}

	void LoadDebugShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 10, 0, true));
		attribs.push_back(ShaderProgram::Attribute("scale", 2, GL_FLOAT, sizeof(float), false, 10, 2, true));
		attribs.push_back(ShaderProgram::Attribute("rotation", 1, GL_FLOAT, sizeof(float), false, 10, 4, true));
		attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(float), false, 10, 5, true));
		attribs.push_back(ShaderProgram::Attribute("shape", 1, GL_FLOAT, sizeof(float), false, 10, 9, true));

		debugShader = LoadShaders(path + "debug.vert", path + "debug.geo", path + "debug.frag", attribs);

		if (!debugShader->wasCompiled())
			FailedCompile();
	}

	void LoadEditorSpriteShader()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 4, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), true, 4, 2));

		editorSpriteShader = LoadShaders(path + "editorsprite.vert", path + "editorsprite.frag", attribs);

		if (!editorSpriteShader->wasCompiled())
			FailedCompile();
	}

	void LoadScreenShaders()
	{
		std::vector<ShaderProgram::Attribute> attribs;
		attribs.push_back(ShaderProgram::Attribute("pos", 2, GL_FLOAT, sizeof(float), false, 4, 0));
		attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(float), false, 4, 2));

		ScreenShader::Default = LoadShaders(path + "screenDefault.vert", path + "screenDefault.frag", attribs);
		if (!ScreenShader::Default->wasCompiled())
			FailedCompile();

		ScreenShader::HDR = LoadShaders(path + "screenDefault.vert", path + "screenHDR.frag", attribs);
		if (!ScreenShader::HDR->wasCompiled())
			FailedCompile();

		ScreenShader::Blur = LoadShaders(path + "screenDefault.vert", path + "screenBlur.frag", attribs);
		if (!ScreenShader::Blur->wasCompiled())
			FailedCompile();

		ScreenShader::BlurCorners = LoadShaders(path + "screenDefault.vert", path + "screenBlurCorners.frag", attribs);
		if (!ScreenShader::BlurCorners->wasCompiled())
			FailedCompile();

		ScreenShader::EdgeDetection = LoadShaders(path + "screenDefault.vert", path + "screenEdgeDect.frag", attribs);
		if (!ScreenShader::EdgeDetection->wasCompiled())
			FailedCompile();

		ScreenShader::Sharpen= LoadShaders(path + "screenDefault.vert", path + "screenSharpen.frag", attribs);
		if (!ScreenShader::Sharpen->wasCompiled())
			FailedCompile();

		ScreenShader::ExtractBrights = LoadShaders(path + "screenDefault.vert", path + "screenExtractBrights.frag", attribs);
		if (!ScreenShader::ExtractBrights->wasCompiled())
			FailedCompile();

		ScreenShader::Bloom = LoadShaders(path + "screenDefault.vert", path + "screenBloom.frag", attribs);
		if (!ScreenShader::Bloom->wasCompiled())
			FailedCompile();

		ScreenShader::Raindrop = LoadShaders(path + "raindrop.vert", path + "raindrop.frag", attribs);
		if (!ScreenShader::Raindrop->wasCompiled())
			FailedCompile();

		ScreenShader::Dropshadow = LoadShaders(path + "screenDefault.vert", path + "screenDropshadow.frag", attribs);
		if (!ScreenShader::Dropshadow->wasCompiled())
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
		GLuint index = glGetUniformBlockIndex(spriteShader->GetProgramID(), "Matrices");
		glUniformBlockBinding(spriteShader->GetProgramID(), index, 0);
	}

	// Creates all basic shaders
	void Init()
	{
		LoadSpriteShader();
		LoadTextShader();
		LoadParticleShaders();
		LoadBackgroundShader();
		LoadDebugShader();
		LoadEditorSpriteShader();
		LoadScreenShaders();

		LoadDefaultShaderUniforms();
		LoadUniformBlockBindings();
	}

	// Frees all basic shaders
	void Unload()
	{
		delete spriteShader;
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


