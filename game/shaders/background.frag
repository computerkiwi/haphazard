#version 330 core

in vec2 texCoord;
in vec4 texBounds;
flat in uint texLayer;

out vec4 FragColor;

uniform sampler2DArray tex;

void main()
{
	//float y = texCoord.y - int(texCoord.y); // Used for vertical parallaxing
	
	float x = texCoord.x - int(texCoord.x);
	if(x < 0)
		x += 1;


	vec2 coord = texBounds.xy + ( vec2(x, texCoord.y) * (texBounds.zw - texBounds.xy));

	vec4 texColor = texture(tex, vec3(coord,texLayer));

	if(texColor.a < 0.1)
		discard;

	FragColor = texColor;
}