#version 330 core

// Vertex
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;

// Character
layout (location = 3) in vec2 charPos;
layout (location = 4) in vec4 texBox;
layout (location = 5) in vec4 color;

layout (std140) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 proj;
};

out vec4 Color;
out vec2 Texcoord;
flat out int TexLayer;

uniform mat4 model;
uniform int font;

void main()
{
	Texcoord = texBox.xy + (texcoord * (texBox.zw - texBox.xy));
	TexLayer = font;

	Color = color;
	gl_Position = proj * view * model * vec4(pos.xy + charPos.xy, pos.z, 1.0);
}
