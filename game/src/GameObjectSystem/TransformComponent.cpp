/*
FILE: TransformComponent.cpp
PRIMARY AUTHOR: Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"
#include "Component.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& scale, float rotation)
	: m_localPosition(position), m_scale(scale), m_rotation(rotation), m_parent(0)
{
}

//---------
// Getters
//---------
void TransformComponent::SetParent(GameObject parent)
{
	if (m_parent)
	{
		Unparent();
	}

	if (parent && parent.GetComponent<TransformComponent>().IsValid())
	{
		glm::vec2 parentPos = parent.GetComponent<TransformComponent>()->GetPosition();
		glm::vec2 diff = GetPosition() - parentPos;
		m_localPosition.x = diff.x;
		m_localPosition.y = diff.y;
	}

	m_parent = parent;
}


void TransformComponent::SetParentLua(GameObject parent)
{
	if (parent && parent.GetComponent<HierarchyComponent>().Get() == nullptr)
	{
		parent.AddComponent<HierarchyComponent>();
	}

	SetParent(parent);
}


GameObject TransformComponent::GetParent() const
{
	return m_parent;
}


float TransformComponent::GetRotation() const
{
	float parentRot = 0;
	if (m_parent)
	{
		parentRot = m_parent.GetComponent<TransformComponent>()->GetRotation();
	}

	return m_rotation + parentRot;
}


void TransformComponent::SetRotation(const float& rotation)
{
	m_rotation = rotation;
}


glm::vec3& TransformComponent::GetRelativePosition()
{
	return m_localPosition;
}


glm::vec2 TransformComponent::GetPosition() const
{
	if (m_parent)
	{
		glm::vec4 affineLocalPos(m_localPosition.x, m_localPosition.y, 0, 1);
		glm::vec2 rotatedPos = glm::rotate(glm::mat4(), DegToRad(m_parent.GetComponent<TransformComponent>()->GetRotation()), glm::vec3(0, 0, 1)) * affineLocalPos;
		return rotatedPos + GetParentPosition();
	}
	else
	{
		return static_cast<glm::vec2>(m_localPosition);
	}
}

glm::vec2 TransformComponent::GetLocalPosition() const
{
	return m_localPosition;
}


void TransformComponent::SetPosition(const glm::vec2& position)
{
	glm::vec2 parentPos = GetParentPosition();

	m_localPosition.x = position.x - parentPos.x;
	m_localPosition.y = position.y - parentPos.y;
}

void TransformComponent::SetLocalPosition(const glm::vec2 &position)
{
	m_localPosition.x = position.x;
	m_localPosition.y = position.y;
}


float TransformComponent::GetZLayer() const
{
	return m_localPosition.z;
}


void TransformComponent::SetZLayer(float layer)
{
	m_localPosition.z = layer;
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
	return glm::translate(glm::mat4(), glm::vec3(GetPosition(), 1)) * glm::rotate(glm::mat4(), DegToRad(GetRotation()), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(), m_scale);
}

void TransformComponent::Unparent()
{
	if (m_parent)
	{
		glm::vec2 pos = GetPosition();
		m_localPosition.x = pos.x;
		m_localPosition.y = pos.y;

		m_parent = 0;
	}
}

glm::vec2 TransformComponent::GetParentPosition() const
{
	if (m_parent.IsValid())
	{
		ComponentHandle<TransformComponent> transform = m_parent.GetComponent<TransformComponent>();
		if (transform->GetParent() == m_parent) // Hacky fix.
		{
			transform->SetParent(INVALID_GAMEOBJECT_ID);
			return glm::vec2();
		}
		else
		{
			return transform->GetPosition();
		}
	}
	else
	{
		// Const cast because we're not changing the state. No data is really changed.
		const_cast<TransformComponent *>(this)->m_parent = INVALID_GAMEOBJECT_ID;
		return glm::vec2(0, 0);
	}
}

