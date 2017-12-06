/*
FILE: screenHDR.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float Exposure;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, TexCoord).rgb;

	// Reinhard tone mapping : vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
	// Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * Exposure);
	

    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(hdrColor,1); //vec4(mapped, 1.0);
}
