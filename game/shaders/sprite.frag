/*
FILE: sprite.frag
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core

in vec4 Color;
in vec2 Texcoord;
in vec4 TexBounds;
flat in int TexLayer;

out vec4 outColor;

uniform sampler2DArray tex;

void main()
{
	vec2 coord = TexBounds.xy + ( vec2(Texcoord.x - int(Texcoord.x), Texcoord.y - int(Texcoord.y)) * (TexBounds.zw - TexBounds.xy));

	vec4 texColor = texture(tex, vec3(coord,TexLayer));

	if(texColor.a < 0.1)
		discard;

	outColor = texColor * Color;
}
