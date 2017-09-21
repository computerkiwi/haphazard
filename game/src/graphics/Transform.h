#pragma once
#include <glm/glm.hpp>



class Transform
{
public:
	Transform();
	Transform(glm::vec3 pos, glm::vec3 rotDegrees, glm::vec3 modelScale);

	void SetPosition(glm::vec3 pos);
	void SetRotation(glm::vec3 rotDegrees);
	void SetScale(glm::vec3 scl);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	glm::mat4 GetMatrix();
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetRotation() { return rotation; }
	glm::vec3 GetScale() { return scale; }

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 matrix;
	bool isDirty = true;
};
