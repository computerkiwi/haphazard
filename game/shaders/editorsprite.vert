/*
FILE: editorsprite.vert
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#version 430 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;

uniform vec2 Position;
uniform vec2 Scale;
uniform float Rotation;

uniform uint TexLayer;
uniform vec4 TexBox;

layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

out vec3 Texcoord;

void main()
{
	float angle = Rotation;

	mat4 s, t, r;
	s[0] = vec4( Scale.x, 0, 0, 0); // Col 1
	s[1] = vec4( 0, Scale.y, 0, 0); // Col 2
	s[2] = vec4( 0, 0, 1, 0); // Col 3
	s[3] = vec4( 0, 0, 0, 1); // Col 4

	
	t[0] = vec4(1,0,0,0); // Col 1
	t[1] = vec4(0,1,0,0); // Col 2
	t[2] = vec4(0,0,1,0);
	t[3] = vec4(Position.x,Position.y,0,1); // Col 4


	r[0] = vec4(cos(angle), sin(angle),0,0); // Col 1
	r[1] = vec4(-sin(angle), cos(angle),0,0); // Col 2
	r[2] = vec4(0,0,1,0);
	r[3] = vec4(0,0,0,1);
	
	mat4 model = t * r * s;

	gl_Position = proj * view * model * vec4(pos + Position, 0,1);

	Texcoord = vec3(TexBox.xy + (texcoord * (TexBox.zw - TexBox.xy)), TexLayer);
}
