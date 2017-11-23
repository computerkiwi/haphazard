#version 330 core
layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec4 SourcePos;
in vec4 Color;
in float Range;
in vec2 Res;

out vec4 outColor;

void main()
{
	vec2 FragPos = gl_FragCoord.xy / Res;

	float Dist = distance(FragPos, SourcePos.xy * (Res.x / Res.y) ); //distance(gl_FragCoord, SourcePos);

	if(Dist > Range)
		discard;

	outColor = Color * (Range - Dist);
	outColor.w = 1;
}
