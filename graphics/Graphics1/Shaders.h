#pragma once

namespace Graphics 
{
	class ShaderProgram;

	namespace Shaders
	{
		void Init();
		void Unload();

		extern ShaderProgram* defaultShader;

		namespace ScreenShader
		{
			extern ShaderProgram* Default;
			extern ShaderProgram* EdgeDetection;
			extern ShaderProgram* Sharpen;
			extern ShaderProgram* Blur;
			extern ShaderProgram* BlurCorners;
			extern ShaderProgram* Bloom;
		}

	};
};


