#include "GL\glew.h"
#include "Background.h"
#include "Shaders.h"

GLuint BackgroundComponent::m_UniTexBox = 0;
GLuint BackgroundComponent::m_UniTexLayer = 0;

static GLuint VAO, VBO;
static bool firstBackground = true;

BackgroundComponent::BackgroundComponent(Texture* texture, BACKGROUND_TYPE type, int layer)
	: m_Texture{texture}, m_Type{type}, m_Layer{layer}
{
	if (firstBackground)
	{
		firstBackground = false;

		Shaders::backgroundShader->Use();
		m_UniTexBox = glGetUniformLocation(Shaders::backgroundShader->GetProgramID(), "TexBox");
		m_UniTexLayer =glGetUniformLocation(Shaders::backgroundShader->GetProgramID(), "TexLayer");
		Texture::BindArray();

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		Shaders::backgroundShader->ApplyAttributes();
		glm::vec4 color = glm::vec4(1,0,0,0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), &color, GL_STATIC_DRAW);

		int e = glGetError();
		if (e)
		{
			glGetError();
		}
	}
}

void BackgroundComponent::SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec4 subTextureBox)
{
	m_ParallaxBounds = glm::vec4(minimumPosition, maximumPosition);
	m_SubTextureBounds = subTextureBox;
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
		box.z = 1;//t.x; // Upper x
		box.w = 1;//t.y; // Upper y
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

		box.x = l.x; // Lower x
		box.y = l.y; // Lower y
		box.z = u.x; // Upper x
		box.w = u.y; // Upper y
		break;
	}

	//Render
	glGetError();
	Shaders::backgroundShader->Use();
	glBindVertexArray(VAO);
	glUniform4f(m_UniTexBox, box.x, box.y, box.z, box.w);
	glUniform1f(m_UniTexLayer, static_cast<float>(m_Texture->GetID()));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	int e = glGetError();
	if (e)
	{
		glGetError();
	}
}