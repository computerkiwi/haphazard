/*
FILE: particleRender.geo
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

out vec2 texcoord;
out float texLayer;
out vec4 color;

in float Type[];
in vec2 Velocity[];
in vec2 Texcoord[];
in float TexLayer[];
in vec4 Color[];
in float Rotation[];
in vec2 Scale[];
in vec2 IResolution[];
in vec4 TexBox[];
in float PercentLife[];

void main() {
	if(Type[0] == 0.0) // Don't render emitter particle
		return;

	color = Color[0];
	texLayer = TexLayer[0];
	
	mat4 rot;
	rot[0] = vec4(cos(Rotation[0]), sin(Rotation[0]), 0, 0);
	rot[1] = vec4(-sin(Rotation[0]), cos(Rotation[0]), 0, 0);
	rot[2] = vec4(0, 0, 1, 0);
	rot[3] = vec4(0, 0, 0, 1);

	vec4 verts[4];
	verts[0] = rot * vec4(-Scale[0].x, Scale[0].y, 0,0);	// TL
	verts[1] = rot * vec4(-Scale[0].x,-Scale[0].y, 0,0);	// BL
	verts[2] = rot * vec4(Scale[0].x, -Scale[0].y, 0,0);	// BR
	verts[3] = rot * vec4(Scale[0].x,  Scale[0].y, 0,0);	// TR

	verts[0].x *= IResolution[0].x; verts[0].y *= IResolution[0].y;
	verts[1].x *= IResolution[0].x; verts[1].y *= IResolution[0].y;
	verts[2].x *= IResolution[0].x; verts[2].y *= IResolution[0].y;
	verts[3].x *= IResolution[0].x; verts[3].y *= IResolution[0].y;

	vec2 texVerts[4];
	texVerts[0] = TexBox[0].xy + (vec2(0,0) * (TexBox[0].zw - TexBox[0].xy));	// TL
	texVerts[1] = TexBox[0].xy + (vec2(0,1) * (TexBox[0].zw - TexBox[0].xy));	// BL
	texVerts[2] = TexBox[0].xy + (vec2(1,1) * (TexBox[0].zw - TexBox[0].xy));	// BR
	texVerts[3] = TexBox[0].xy + (vec2(1,0) * (TexBox[0].zw - TexBox[0].xy));	// TR

	// Triangle 1
    gl_Position = gl_in[0].gl_Position + verts[0];
	texcoord = texVerts[0];
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + verts[1];
	texcoord = texVerts[1];
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + verts[2];
	texcoord = texVerts[2];
    EmitVertex();

	// Triangle 2
	gl_Position = gl_in[0].gl_Position + verts[2];
	texcoord = texVerts[2];
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + verts[0];
	texcoord = texVerts[0];
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + verts[3];
	texcoord = texVerts[3];
    EmitVertex();

    EndPrimitive();
}  