/*
FILE: raindrop.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord;
in vec2 Offset;
in float Alpha;
in float Depth;

out vec4 FragColor;

uniform sampler2D raindropTexture;
uniform sampler2D screenTexture;

void main()
{
	if(length(texture(raindropTexture, TexCoord).xyz) <= 0) // Is black (invisible with additive rendering)
		discard;


	vec2 normCoord = 2.0 * TexCoord.xy - 1.0;

	// Get polar coords
	float r = length(normCoord);
	float phi = atan(normCoord.y, normCoord.x);

	// Bulge radius
	// Grow radius to 0.5 in squared rate then continue linearly after 0.5
	// Optional: change bounds from 1 to alpha of raindrop texture
	r = r * smoothstep(-0.1, 0.7 * Depth, r);

	// Convert back
	normCoord.x = r * cos(phi); 
	normCoord.y = r * sin(phi); 
	vec2 texCoord = (normCoord + 1) / 2.0;
	texCoord.y = 1 - texCoord.y; // Flip screen texture

	vec4 Color = texture(screenTexture, texCoord);
	Color *= texture(raindropTexture, TexCoord);
	Color.a = length(Color.xyz);
	Color.a *= Alpha;

	FragColor = Color; // Add color to blend overlapping raindrops better
}
