#pragma once

namespace Graphics 
{
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
		}

	};
};


