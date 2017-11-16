#include "GL\glew.h"
#include "Background.h"
#include "Shaders.h"
#include "Camera.h"

GLuint BackgroundComponent::m_UniTexBox = 0;
GLuint BackgroundComponent::m_UniTexLayer = 0;
Screen::Mesh* BackgroundComponent::m_Mesh;

static GLuint VAO, VBO;
static bool firstBackground = true;

BackgroundComponent::BackgroundComponent(Texture* texture, BACKGROUND_TYPE type)
	: m_Texture{texture}, m_Type{type}
{
	if (firstBackground)
	{
		firstBackground = false;

		Shaders::backgroundShader->Use();
		m_UniTexBox = glGetUniformLocation(Shaders::backgroundShader->GetProgramID(), "TexBox");
		m_UniTexLayer =glGetUniformLocation(Shaders::backgroundShader->GetProgramID(), "TexLayer");
		Texture::BindArray();
		// Make screen mesh "upside down" so sprites are rendered in correct orientation
		m_Mesh = new Screen::Mesh(1,0,0,1);
	}
}

void BackgroundComponent::SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec2 subTextureSize, glm::vec2 subTexturePos)
{
	m_ParallaxBounds = glm::vec4(minimumPosition, maximumPosition);
	m_SubTextureSize = subTextureSize;
	m_SubTexturePosition = subTexturePos;
}

void BackgroundComponent::Render(glm::vec2 pos)
{
	if(!m_Texture)
		return;

	glm::vec4 box;
	glm::vec2 t = m_Texture->GetBounds();

	switch (m_Type)
	{
	case BACKGROUND_TYPE::BACKGROUND:
	case BACKGROUND_TYPE::FOREGROUND:
		box.x = 0; // Lower x
		box.y = 0; // Lower y
		box.z = t.x; // Upper x
		box.w = t.y; // Upper y
		break;
	case BACKGROUND_TYPE::BACKGROUND_PARALLAX:
	case BACKGROUND_TYPE::FOREGROUND_PARALLAX:
		// Get progress in x and y directions
    glm::vec2 p = glm::vec2( (pos.x - m_ParallaxBounds.x) / (m_ParallaxBounds.z - m_ParallaxBounds.x),
                             (pos.y - m_ParallaxBounds.y) / (m_ParallaxBounds.w - m_ParallaxBounds.y) );


		if (m_ParallaxBounds.z == 0 || p.x > 1)
			p.x = 1;
		if (m_ParallaxBounds.w == 0 || p.y > 1)
			p.y = 1;
		// Linearly interpolate between minimum and maximum

		// Lower bounds
    glm::vec2 l = glm::vec2( (m_TextureXRange.x + p.x * (m_TextureXRange.y - m_TextureXRange.x)) * t.x, // Percent from 0 to max
                             (m_TextureYRange.x + p.y * (m_TextureYRange.y - m_TextureYRange.x)) * t.y);

		// Upper bound (lower + offset)
    glm::vec2 u = glm::vec2(t.x * m_SubTextureSize.x + l.x,
                            t.y * m_SubTextureSize.y / Camera::GetActiveCamera()->GetAspectRatio() + l.y);

		box.x = l.x + m_SubTexturePosition.x * t.x; // Lower x (Left)
		box.y = l.y - (1 - m_SubTexturePosition.y) * t.y; // Lower y (Bottom)
		box.z = u.x + m_SubTexturePosition.x * t.x; // Upper x (Right)
		box.w = u.y - (1 - m_SubTexturePosition.y) * t.y; // Upper y (Top)
		break;
	}

	//Render
	Shaders::backgroundShader->Use();
	m_Mesh->Bind();
	Texture::BindArray();
	glUniform4f(m_UniTexBox, box.x, box.y, box.z, box.w);
	glUniform1ui(m_UniTexLayer, m_Texture->GetID());
	m_Mesh->DrawTris();
}