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


void TransformComponent::SetParentLua(GameObject parent)
{
	if (parent && parent.GetComponent<HierarchyComponent>().Get() == nullptr)
	{
		parent.AddComponent<HierarchyComponent>();
	}
	m_parent = parent;
}


GameObject TransformComponent::GetParent() const
{
	return m_parent;
}


float TransformComponent::GetRotation() const
{
	return m_rotation;
}


void TransformComponent::SetRotation(const float& rotation)
{
	m_rotation = rotation;
}


glm::vec3& TransformComponent::GetRelativePosition()
{
	return m_position;
}


glm::vec2 TransformComponent::GetPosition() const
{
	if (m_parent)
	{
		return static_cast<glm::vec2>(m_position) + m_parent.GetComponent<TransformComponent>()->GetPosition();
	}

	return m_position;
}


void TransformComponent::SetPosition(const glm::vec2& position)
{
	m_position.x = position.x;
	m_position.y = position.y;
}


float TransformComponent::GetZLayer() const
{
	return m_position.z;
}


void TransformComponent::SetZLayer(float layer)
{
	m_position.z = layer;
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
	return glm::translate(glm::mat4(), glm::vec3(GetPosition(), 1)) * glm::rotate(glm::mat4(), DegToRad(m_rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(), m_scale);
}

