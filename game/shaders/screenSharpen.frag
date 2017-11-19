#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float Intensity;

vec4 applyKernel(float[9] kernel, float amt)
{
	const float offset = 1.0 / 300.0;

	for(int i = 0; i < 9; i++) kernel[i] *= amt;

	vec4 col = vec4(0.0);
	for(int i = 0; i < 9; i++)
	{
		vec2 off = vec2((i / 3 - 1) * offset, (i % 3 - 1) * offset); //Intentionaly inversed x and y
		col += vec4(texture(screenTexture, TexCoord + off)) * kernel[i];
	}
	return vec4(col);
}

vec4 sharpen(float amt)
{
	float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

	return applyKernel(kernel, amt);
}

void main()
{
	FragColor = sharpen(Intensity);
}
