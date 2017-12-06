/*
FILE: screenBloom.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D brights;

void main()
{
    vec4 sceneColor = texture(screenTexture, TexCoord);
    vec4 bloomColor = texture(brights, TexCoord);
    
	//FragColor = sceneColor + bloomColor;

    FragColor = sceneColor * (1 - bloomColor.a) + bloomColor;
}