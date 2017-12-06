/*
FILE: debug.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core
out vec4 FragColor;

in vec4 Color;

in VS_OUT {
    vec4[4] Offsets;
	vec2 Scale;
	vec4 Color;
} gs_in[];

void main()
{
    FragColor = Color;// * Color.a;
}  