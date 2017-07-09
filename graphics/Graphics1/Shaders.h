#pragma once

class ShaderProgram;

namespace Shaders
{
	void Init();
	void Unload();

	extern ShaderProgram* defaultShader;
	extern ShaderProgram* defaultScreenShader;

};
