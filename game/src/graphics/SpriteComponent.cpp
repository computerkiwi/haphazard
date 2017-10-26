#include "SpriteComponent.h"

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
		SetTexture(GetDefaultTexture());
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

	if (t)
		SetTexture(t, fps);

	m_resID = -1;
}

void SpriteComponent::SetTextureResource(Resource *res)
{
	if (res == nullptr)
	{
		SetTexture(GetDefaultTexture());
		m_resID = -1;
		return;
	}

	assert(res->GetResourceType() == ResourceType::TEXTURE);
	Texture *tex = reinterpret_cast<Texture *>(res->Data());
	SetTexture(tex);
	m_resID = res->Id();
}


void SpriteComponent::SetTextureID(Resource *res)
{

}
