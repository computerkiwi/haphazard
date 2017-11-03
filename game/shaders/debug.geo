#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 5) out;

out vec4 Color;

in VS_OUT {
    vec4[4] Offsets;
	vec2 Scale;
	vec4 Color;
} gs_in[];

void main() {
	Color = gs_in[0].Color;

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