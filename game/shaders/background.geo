#version 330 core
layout (triangles) in;
layout (points, max_vertices = 4) out;

in vec4 texBox[];
in float texLayer[];

out vec3 Texcoord;
//out vec4 Color;

void main() {
	//Color = color[0];

	vec2 texVerts[4];
	texVerts[0] = texBox[0].xy + (vec2(0,0) * (texBox[0].zw - texBox[0].xy));	// TL
	texVerts[1] = texBox[0].xy + (vec2(0,1) * (texBox[0].zw - texBox[0].xy));	// BL
	texVerts[2] = texBox[0].xy + (vec2(1,1) * (texBox[0].zw - texBox[0].xy));	// BR
	texVerts[3] = texBox[0].xy + (vec2(1,0) * (texBox[0].zw - texBox[0].xy));	// TR

	// Triangle 1
    gl_Position = vec4(1,1,0,0); //TL
	Texcoord = vec3(texVerts[0], 6);
    EmitVertex();

	gl_Position = vec4(-1,1,0,0); // TL
	Texcoord = vec3(texVerts[1], 6);
    EmitVertex();

	gl_Position = vec4(1,-1,0,0); // TL
	Texcoord = vec3(texVerts[2], 6);
    EmitVertex();

	// Triangle 2
	gl_Position = vec4(-1,-1,0,0); // BL
	Texcoord = vec3(texVerts[2], 6);
    EmitVertex();

    EndPrimitive();
}