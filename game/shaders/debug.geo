/*
FILE: debug.geo
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 30) out;

#define SQUARE 0.0f
#define CIRCLE 1.0f

#define PI 3.14159265

out vec4 Color;

in VS_OUT {
    vec4[4] Offsets;
	vec2 Scale;
	vec4 Color;
	float Type;
} gs_in[];

void main() {
	Color = gs_in[0].Color;

	if(gs_in[0].Type == SQUARE)
	{
		vec2 Scale = gs_in[0].Scale; // / 2;
	
		gl_Position = gs_in[0].Offsets[0]; //gl_in[0].gl_Position + vec4(-Scale.x, -Scale.y, 0,0);
		EmitVertex();

		gl_Position = gs_in[0].Offsets[1]; //gl_in[0].gl_Position + vec4(-Scale.x, Scale.y, 0,0);
		EmitVertex();

		gl_Position = gs_in[0].Offsets[2]; //gl_in[0].gl_Position + vec4(Scale.x, Scale.y, 0,0);
		EmitVertex();

		gl_Position = gs_in[0].Offsets[3]; //gl_in[0].gl_Position + vec4(Scale.x, -Scale.y, 0,0);
		EmitVertex();

		gl_Position = gs_in[0].Offsets[0]; //gl_in[0].gl_Position + vec4(-Scale.x, -Scale.y, 0,0);
		EmitVertex();

		EndPrimitive();
	}
	else
	{
		float r = gs_in[0].Offsets[1].x - gs_in[0].Offsets[0].x;
		// Scale.x = radius, Scale.y = step
		for(float i = 0; i < 2 * PI; i += 0.3f) 
		{
			gl_Position = gs_in[0].Offsets[0] + vec4( r * cos(i), r * sin(i) * gs_in[0].Scale.y,0,0);
			EmitVertex();
		}
		gl_Position = gs_in[0].Offsets[0] + vec4(r,0,0,0); // initial point
		EmitVertex();

		EndPrimitive();
	}

}  