#version 330 core

in vec2 texCoord;
in vec4 texBounds;
flat in uint texLayer;

out vec4 FragColor;

uniform sampler2DArray tex;

void main()
{
	vec2 coord = texBounds.xy + ( vec2(texCoord.x - int(texCoord.x), texCoord.y - int(texCoord.y)) * (texBounds.zw - texBounds.xy));

	vec4 texColor = texture(tex, vec3(coord,texLayer));

	if(texColor.a < 0.1)
		discard;

	FragColor = texColor;
}