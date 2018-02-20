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
	bool tiled = fract(TexBounds) != TexBounds; // Flag (+1 to all bounds) is set if texture is tiled

	vec2 coord = TexBounds.xy + (fract(Texcoord) * (TexBounds.zw - TexBounds.xy));

	if(tiled)
		coord = clamp(coord,TexBounds.xy + 0.001, TexBounds.zw - 0.001);

	vec4 texColor = texture(tex, vec3(coord,TexLayer));
	if(texColor.a < 0.1)
		discard;
	outColor = texColor * Color;
}
