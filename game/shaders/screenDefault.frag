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

	if (texColor.a < 0.8)
	{
		// Get the neighbouring four pixels.
		vec4 pixelDown  = texture(screenTexture, vec2(TexCoord + vec2(0, 0.005)) );

		// If one of the neighbouring pixels is invisible, we render an outline.
		if (pixelDown.a > 0 )    // * pixelDown.a * pixelRight.a * pixelLeft.a <= 0.1) 
		{
			texColor = vec4(0, 0, 0, 0.5);
		}
	}

	FragColor = texColor;
}
