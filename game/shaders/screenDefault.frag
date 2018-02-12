/*
FILE: screenDefault.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{ 
	vec4 texColor = texture(screenTexture, TexCoord);

	if (texColor.a >= 0.1)
	{
		texColor = vec4(0, 0, 0, 0);

		texColor.a += ceil(texture(screenTexture, vec2(TexCoord + vec2(0, 0.005)) ).a) * 0.1;
		texColor.a += ceil(texture(screenTexture, vec2(TexCoord + vec2(0, 0.004)) ).a) * 0.1;
		texColor.a += ceil(texture(screenTexture, vec2(TexCoord + vec2(0, 0.003)) ).a) * 0.1;
		texColor.a += ceil(texture(screenTexture, vec2(TexCoord + vec2(0, 0.002)) ).a) * 0.1;
		texColor.a += ceil(texture(screenTexture, vec2(TexCoord + vec2(0, 0.001)) ).a) * 0.1;
	}

	FragColor = texColor;
}
