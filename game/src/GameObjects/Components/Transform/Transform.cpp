/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Transform.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() : Transform(nullptr, glm::vec3(), glm::vec3(), 0.0f)
{
}


Transform::Transform(GameObject * parent) : Transform(parent, glm::vec3(), glm::vec3(), 0.0f)
{
}



Transform::Transform(GameObject * parent, glm::vec3 & position, glm::vec3 & scale)
								: Component(parent), m_Position(position), m_Scale(scale), m_Rotation(0.0f)
{
}


Transform::Transform(GameObject * parent, glm::vec3 && position, glm::vec3 && scale)
								: Transform(parent, position, scale, 0.0f)
{
}



Transform::Transform(GameObject * parent, glm::vec3 & position, glm::vec3 & scale, float rotation)
								: Component(parent), m_Position(position), m_Scale(scale), m_Rotation(rotation)
{
}


Transform::Transform(GameObject * parent, glm::vec3 && position, glm::vec3 && scale, float rotation)
								: Component(parent), m_Position(position), m_Scale(scale), m_Rotation(rotation)
{
}



glm::vec3 Transform::GetPosition() const
{
	assert(mParent && "Component does not have a parent!!");
	return m_Position;
}


void Transform::SetPosition(glm::vec3 position)
{
	assert(mParent && "Component does not have a parent!!");
	isDirty = true;
	m_Position = position;
}


glm::vec3 Transform::GetScale() const
{
	assert(mParent && "Component does not have a parent!!");
	return m_Scale;
}


void Transform::SetScale(glm::vec3 scale)
{
	assert(mParent && "Component does not have a parent!!");
	isDirty = true;
	m_Scale = scale;
}


glm::mat4 Transform::GetMatrix()
{
	assert(mParent && "Component does not have a parent!!");
	glm::mat4 matrix;

	if (isDirty)
	{
		matrix = glm::translate(matrix, m_Position);
		matrix = glm::rotate(matrix, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, m_Scale);

		isDirty = false;
	}
	return matrix;
}


const glm::vec3 & Transform::GetRotation() const
{
	assert(mParent && "Component does not have a parent!!");
	return m_Rotation;
}


void Transform::SetRotation(const glm::vec3 & rotation)
{
	assert(mParent && "Component does not have a parent!!");
	isDirty = true;
	m_Rotation = rotation;
}


