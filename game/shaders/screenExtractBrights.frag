/*
FILE: screenExtractBrights.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoord);

	//float bright = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722)); 
	float bright = dot(FragColor.rgb, vec3(1,1,1)); 

	if(bright > 0.75) // If above threshold
		FragColor = texture(screenTexture, TexCoord);
	else
		FragColor = vec4(0,0,0,0);
}
