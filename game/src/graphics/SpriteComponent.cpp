/*
FILE: SpriteComponent.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "SpriteComponent.h"
#include "TextureResource.h" // For INVALID_TEXTURE_ID


// Constructs a unit mesh (1x1 white mesh) and sets texture to be used for that mesh
SpriteComponent::SpriteComponent(Resource *res)
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

	if (res)
	{
		SetTextureResource(res);
	}
	else
	{
		// If no texture is given, display default texture (solid color)
		SetTextureResource(engine->GetResourceManager().Get("default.png"));
	}
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

	// Animated texture should be set to something, null should never be passed
	if (t)
		SetTexture(t, fps);
	else
		assert(!"You passed a NULL animated texture to a sprite component");

	m_resID = INVALID_TEXTURE_ID;
}

void SpriteComponent::SetTextureResource(Resource *res)
{
	if (res == nullptr)
	{
		SetTexture(GetDefaultTexture());
		m_resID = INVALID_TEXTURE_ID;
		return;
	}

	assert(res->GetResourceType() == ResourceType::TEXTURE);
	Texture *tex = reinterpret_cast<Texture *>(res->Data());
	SetTexture(tex);
	m_resID = res->Id();
}


void SpriteComponent::SetTextureID(ResourceID res)
{
	if (res == INVALID_TEXTURE_ID)
	{
		SetTexture(GetDefaultTexture());
		m_resID = INVALID_TEXTURE_ID;
		return;
	}

	Resource *resource = engine->GetResourceManager().Get(res);

	assert(resource->GetResourceType() == ResourceType::TEXTURE);
	Texture *tex = reinterpret_cast<Texture *>(resource->Data());
	SetTexture(tex);
	m_resID = res;
}
