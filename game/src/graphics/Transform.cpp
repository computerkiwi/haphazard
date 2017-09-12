#include "Transform.h"

#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

///
// Transform
///

Graphics::Transform::Transform()
	: scale{ glm::vec3(1,1,1) }
{ }

Graphics::Transform::Transform(glm::vec3 pos, glm::vec3 rotDegrees, glm::vec3 modelScale)
	: position{ pos }, rotation{ rotDegrees }, scale{ modelScale }
{ }

glm::mat4 Graphics::Transform::GetMatrix()
{
	if (isDirty)
	{
		matrix = glm::mat4();
		matrix = glm::translate(matrix, position);
		matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, scale);

		isDirty = false;
	}
	return matrix;
}

void Graphics::Transform::SetPosition(glm::vec3 pos)
{
	position = pos;
	isDirty = true;
}

void Graphics::Transform::SetRotation(glm::vec3 rotDegrees)
{
	rotation = rotDegrees;
	isDirty = true;
}

void Graphics::Transform::SetScale(glm::vec3 scl)
{
	scale = scl;
	isDirty = true;
}

void Graphics::Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	isDirty = true;
}

void Graphics::Transform::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	isDirty = true;
}

void Graphics::Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	isDirty = true;
}