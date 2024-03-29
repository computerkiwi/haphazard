/*
FILE: screenBlurCorners.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float Intensity;

vec4 applyKernel(float[9] kernel, float amt)
{
	const float offset = 1.0 / 300.0;

	for(int i = 0; i < 9; i++) kernel[i] *= amt;

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		vec2 off = vec2((i / 3 - 1) * offset, (i % 3 - 1) * offset); //Intentionaly inversed x and y
		col += vec3(texture(screenTexture, TexCoord + off)) * kernel[i];
	}
	return vec4(col,1);
}

const vec2 gaussFilter[7] = vec2[](
	vec2(-3.0,	0.015625),
	vec2(-2.0,	0.09375),
	vec2(-1.0,	0.234375),
	vec2(0.0,	0.3125),
	vec2(1.0,	0.234375),
	vec2(2.0,	0.09375),
	vec2(3.0,	0.015625)
);

vec4 blurCorners(float amtX, float amtY)
{
	amtX /= 1000.0;
	amtY /= 1000.0;
	
	float weight = abs(-0.5 + TexCoord.x) * abs(-0.5 + TexCoord.y);
	amtX *= weight;
	amtY *= weight;

	vec4 color = vec4(0,0,0,0);
	for(int i = 0; i < 7; i++)
	{
		color += gaussFilter[i].y * texture( screenTexture, vec2(TexCoord.x + gaussFilter[i].x * amtX, TexCoord.y + gaussFilter[i].x * amtY) );
	}

	return color;
}

void main()
{
	FragColor = blurCorners(30 * Intensity, 30 * Intensity);
}
