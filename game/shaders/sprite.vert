/*
FILE: sprite.vert
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 vcolor;
layout (location = 2) in vec2 texcoord;

// Instanced
layout (location = 3) in vec4 color;
layout (location = 4) in vec4 texBox;
layout (location = 5) in mat4 model;
// location 5,6,7,8 used by model
layout (location = 9) in vec2 texScale;

layout (location = 10) in int texLayer;

out vec4 Color;
out vec2 Texcoord;
out vec4 TexBounds;
flat out int TexLayer;

layout (std140, binding = 1) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

void main()
{
						  // texScale.xy is scale of object
	Texcoord = texcoord  * texScale.xy;   //texBox.xy + (texcoord * (texBox.zw - texBox.xy));
	TexBounds = texBox;
	TexLayer = texLayer;

	Color = color * vcolor;
	gl_Position = proj * view * model * vec4(pos, 1.0);
}
