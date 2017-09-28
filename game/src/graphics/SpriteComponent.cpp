#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(Texture* t)
  : Mesh()
{
	AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f,  0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 1, // Top Left
		-0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 0, // Bot Left
		0.5f,  -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 0  // Bot Right
	);
	AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		0.5f,  -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 0, // Bot Right
		0.5f,   0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 1, // Top Right
		-0.5f,  0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 1  // Top Left
	);
	CompileMesh();

	if(t)
		SetTexture(t);
}

SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps)
{
	AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 1, // Top Left
		-0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 0, // Bot Left
		0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 0  // Bot Right
	);
	AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 0, // Bot Right
		0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 1, 1, // Top Right
		-0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 1, 0, 1  // Top Left
	);
	CompileMesh();

	if (t)
		SetTexture(t, fps);
}
