/*
FILE: TransformComponent.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include "GameObject.h"

// TODO[Kiera] - Move this somewhere more general.
constexpr float DegToRad(float degrees)
{
	const float PI = 3.14159265358979323846264338327950f;
	const float RADIANS_IN_CIRCLE = 2 * PI;
	const float DEGREES_IN_CIRCLE = 360.0f;

	return degrees * RADIANS_IN_CIRCLE / DEGREES_IN_CIRCLE;
}

class Editor;

class TransformComponent
{
public:
	//--------------
	// Constructors
	//--------------

	TransformComponent(const glm::vec3& position = glm::vec3(0.0f),
					   const glm::vec3& scale = glm::vec3(1.0f),
					   float rotation = 0.0f);

	//---------
	// Getters
	//---------

	void SetParent(GameObject parent);
	void SetParentLua(GameObject parent);
	GameObject GetParent() const;

	float GetRotation() const;

	void SetRotation(const float& rotation);

	glm::vec3& GetRelativePosition();

	void SetZLayer(float layer);
	float GetZLayer() const;

	glm::vec2 GetPosition() const;
	glm::vec2 GetLocalPosition() const;

	void SetPosition(const glm::vec2& position);
	void SetLocalPosition(const glm::vec2& position);

	glm::vec3 GetScale() const;

	void SetScale(const glm::vec3& scale);

	glm::vec2 Scale2D() const;

	glm::mat4 GetMatrix4() const;

private:
	void Unparent();

	glm::vec2 GetParentPosition() const;

	friend void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor);

	glm::vec3 m_localPosition;
	glm::vec3 m_scale;
	float m_rotation; // Stored in degrees.

	GameObject m_parent;

	static void TransformComponentDeserializeAssign(void *transformPtr, rapidjson::Value& jsonTransform)
	{
		// Get the engine.
		TransformComponent& transform = *reinterpret_cast<TransformComponent *>(transformPtr);

		Assert(jsonTransform.IsObject());

		// Position first so it doesn't remove a bad parent.
		if (jsonTransform.HasMember("position")) { transform.SetPosition(meta::DeserializeConstruct<decltype(transform.GetPosition())>(jsonTransform["position"])); }
		// Set the parent manually so our position doesn't change.
		if (jsonTransform.HasMember("parent")) {transform.m_parent = meta::DeserializeConstruct<decltype(transform.GetParent())>(jsonTransform["parent"]); }
		if (jsonTransform.HasMember("scale")) { transform.SetScale(meta::DeserializeConstruct<decltype(transform.GetScale())>(jsonTransform["scale"])); }
		if (jsonTransform.HasMember("zLayer")) { transform.SetZLayer(meta::DeserializeConstruct<decltype(transform.GetZLayer())>(jsonTransform["zLayer"])); }
		if (jsonTransform.HasMember("rotation")) { transform.SetRotation(meta::DeserializeConstruct<decltype(transform.GetRotation())>(jsonTransform["rotation"])); }

	}

	static rapidjson::Value TransformComponentSerializeFunction(const void *transformPtr, rapidjson::Document::AllocatorType& allocator)
	{
		// Const cast away is fine because we're not really changing anything.
		TransformComponent& transform = *reinterpret_cast<TransformComponent *>(const_cast<void *>(transformPtr));

		// Setup the object to store the engine info in.
		rapidjson::Value jsonOut;
		jsonOut.SetObject();

		
		jsonOut.AddMember("position", meta::Any(transform.GetPosition()).Serialize(allocator), allocator);
		jsonOut.AddMember("scale", meta::Any(transform.GetScale()).Serialize(allocator), allocator);
		jsonOut.AddMember("zLayer", meta::Any(transform.GetZLayer()).Serialize(allocator), allocator);
		jsonOut.AddMember("rotation", meta::Any(transform.GetRotation()).Serialize(allocator), allocator);
		jsonOut.AddMember("parent", meta::Any(transform.GetParent()).Serialize(allocator), allocator);

		return jsonOut;
	}

	META_REGISTER(TransformComponent)
	{
		META_DefineGetterSetter(TransformComponent, glm::vec2, GetPosition, SetPosition, "position");

		META_DefineGetterSetter(TransformComponent, glm::vec2, GetLocalPosition, SetLocalPosition, "localPosition");

		META_DefineGetterSetter(TransformComponent, glm::vec3, GetScale, SetScale, "scale");

		META_DefineGetterSetter(TransformComponent, float, GetZLayer, SetZLayer, "zLayer");

		META_DefineGetterSetter(TransformComponent, float, GetRotation, SetRotation, "rotation");

		META_DefineGetterSetter(TransformComponent, GameObject, GetParent, SetParentLua, "parent");

		META_DefineDeserializeAssignFunction(TransformComponent, TransformComponentDeserializeAssign);

		META_DefineSerializeFunction(TransformComponent, TransformComponentSerializeFunction);
	}
};
