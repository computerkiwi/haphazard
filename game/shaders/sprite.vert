#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texcoord;

// Instanced
layout (location = 3) in vec4 texBox;
layout (location = 4) in mat4 model;
// location 4,5,6,7 used by model

layout (location = 8) in int texLayer;

out vec4 Color;
out vec2 Texcoord;
flat out int TexLayer;

layout (std140) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

void main()
{
	Texcoord = texBox.xy + (texcoord * (texBox.zw - texBox.xy));
	TexLayer = texLayer;

	Color = color;
	gl_Position = proj * view * model * vec4(pos, 1.0);
}
