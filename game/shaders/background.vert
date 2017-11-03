#version 330 core

out vec4 texBox;
out float texLayer;

uniform vec4 TexBox;
uniform float TexLayer;

void main()
{
	texBox = TexBox;
	texLayer = TexLayer;
}
