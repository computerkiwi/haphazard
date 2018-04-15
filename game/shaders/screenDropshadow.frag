/*
FILE: screenDropshadow.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord; 

out vec4 FragColor;
 
uniform sampler2D screenTexture;
uniform float ZoomScale = 1.0;

void main()
{
	vec4 texColor = texture(screenTexture, TexCoord);

	if(texColor.a < 0.6)
	{
		float off = 0.005 / ZoomScale;
		float downOff = 0.005 / ZoomScale;

		vec4 col = texColor;
		texColor = vec4(0,0,0,0);

		// Size
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,-off)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(off,0)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(-off,0)).a) * 0.15;

		// Down
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,downOff * 1)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,downOff * 2)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,downOff * 3)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,downOff * 4)).a) * 0.15;
		texColor.a += step(0.6, texture(screenTexture, TexCoord + vec2(0,downOff * 5)).a) * 0.15;

		if(texColor.a < 0.1)
			texColor = col;
	}

	FragColor = texColor;
}
