#include "DebugGraphic.h"
#include "Shaders.h"

std::list<DebugGraphic*> DebugGraphic::shapes;
int DebugGraphic::amount;
unsigned int DebugGraphic::VAO;
unsigned int DebugGraphic::VBO;

DebugGraphic::DebugGraphic(glm::vec2 pos, int verts)
	: m_position(pos), m_vertices(verts), m_index(amount)
{
	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		Graphics::Shaders::debugShader->ApplyAttributes();
	}

	if(!VBO)
		glGenBuffers(1, &VBO);

	shapes.push_back(this);
	amount++;
}

DebugGraphic::~DebugGraphic()
{
	shapes.remove(this);
	amount--;
}

void DebugGraphic::DebugGraphic::DrawAll()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	Graphics::Shaders::debugShader->Use();

	float *positions = new float(amount * 2); // 2 floats per shape
	int i = 0;

	for (auto shape : shapes)
	{
		positions[i++] = shape->m_position.x;
		positions[i++] = shape->m_position.y;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amount, positions, GL_STATIC_DRAW);

	glDrawArrays(GL_POINTS, 0, 4);
}
