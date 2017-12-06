/*
FILE: text.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec4 Color;
in vec2 Texcoord;
flat in int TexLayer;

out vec4 outColor;

uniform sampler2DArray tex;

void main()
{
	vec4 texColor = texture(tex, vec3(Texcoord,TexLayer));
	if(texColor.a < 0.1)
		discard;
	outColor = texColor * Color;
}
