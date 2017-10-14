/*
FILE: TransformComponent.cpp
PRIMARY AUTHOR: Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"
#include "Component.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& scale, float rotation)
	: m_position(position), m_scale(scale), m_rotation(rotation), m_parent(0)
{
}

//---------
// Getters
//---------

void TransformComponent::SetParent(GameObject parent)
{
	m_parent = parent;
}

GameObject TransformComponent::GetParent() const
{
	return m_parent;
}

float& TransformComponent::Rotation()
{
	return m_rotation;
}

float TransformComponent::GetRotation() const
{
	return m_rotation;
}

void TransformComponent::SetRotation(const float& rotation)
{
	m_rotation = rotation;
}

glm::vec3& TransformComponent::Position()
{
	if (m_parent)
	{
		m_parenting_position = m_position + m_parent.GetComponent<TransformComponent>()->m_position;
		return m_parenting_position;
	}

	return m_position;
}

glm::vec3& TransformComponent::GetRelativePosition()
{
	return m_position;
}

glm::vec3 TransformComponent::GetPosition() const
{
	return m_position;
}

void TransformComponent::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

glm::vec3 TransformComponent::Position() const
{
	return m_position;
}

glm::vec2 TransformComponent::GetPosition2D() const
{
	return m_position;
}

glm::vec3& TransformComponent::Scale()
{
	return m_scale;
}

glm::vec3 TransformComponent::GetScale() const
{
	return m_scale;
}

void TransformComponent::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

glm::vec2 TransformComponent::Scale2D() const
{
	return m_scale;
}

glm::mat4 TransformComponent::GetMatrix4() const
{
	return glm::translate(glm::mat4(), m_position) * glm::rotate(glm::mat4(), DegToRad(m_rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(), m_scale);
}

