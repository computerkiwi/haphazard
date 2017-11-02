#include "Background.h"

BackgroundComponent::BackgroundComponent(Texture* texture, BACKGROUND_TYPE type, int layer)
	: m_Texture{texture}, m_Type{type}, m_Layer{layer}
{
}

void BackgroundComponent::SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec4 subTextureBox)
{
	m_ParallaxBounds = glm::vec4(minimumPosition, maximumPosition);
	m_SubTextureBounds = subTextureBox;
}

void BackgroundComponent::Render(glm::vec2 pos)
{
	float data[4];
	glm::vec2 t = m_Texture->GetBounds();

	switch (m_Type)
	{
	case BACKGROUND_TYPE::BACKGROUND:
	case BACKGROUND_TYPE::FOREGROUND:
		data[0] = 0; // Lower x
		data[1] = 0; // Lower y
		data[2] = t.x; // Upper x
		data[3] = t.y; // Upper y
		break;
	case BACKGROUND_TYPE::PARALLAX:
		// Get progress in x and y directions
		glm::vec2 p = glm::vec2( (pos.x + m_ParallaxBounds.x) / m_ParallaxBounds.z,
														 (pos.y + m_ParallaxBounds.y) / m_ParallaxBounds.w);

		// Linearly interpolate between minimum and maximum 

		// Lower bounds
		glm::vec2 l = glm::vec2(m_SubTextureBounds.x + p.x * (t.x - (m_SubTextureBounds.z - m_SubTextureBounds.x)) ,
														m_SubTextureBounds.y + p.y * (t.y - (m_SubTextureBounds.w - m_SubTextureBounds.y)) );

		// Upper bound (lower + offset)
		glm::vec2 u = glm::vec2(l.x + (m_SubTextureBounds.z - m_SubTextureBounds.x) , 
														l.y + (m_SubTextureBounds.w - m_SubTextureBounds.y) );

		data[0]=l.x; // Lower x
		data[1]=l.y; // Lower y
		data[2]=u.x; // Upper x
		data[3]=u.y; // Upper y
		break;
	}

	//render
}