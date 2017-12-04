/*
FILE: Camera.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include "GL\glew.h"
#include <glm/glm.hpp>

#include "Input\Input.h"

class Editor;
class GameObject;

class Camera
{
public:

	///
	// Basic Camera functionality
	///

	Camera();
	Camera(const Camera& other);
	~Camera();

	void Use() { m_CurrActiveCamera = this; ApplyCameraMatrices(); }
	bool IsActiveCam() const { return this == m_CurrActiveCamera; };

	///
	// Setters
	///

	void SetView(glm::vec3 pos, glm::vec3 target, glm::vec3 upVector);
	void SetProjection(float zoom, float aspectRatio, float near, float far);

	void SetPosition(glm::vec2 pos) { m_Position = m_Center = glm::vec3(pos, 2); m_Center.z = 0; }
	void SetUp(glm::vec3 up) { m_Up = up; }

	void SetRotation(float degrees);

	void SetZoom(float zoom) { m_Zoom = zoom;}
	void SetAspectRatio(float ar) { m_AspectRatio = ar; }
	void SetNearPlane(float near);
	void SetFarPlane(float far);

	///
	// Getters
	///

	glm::vec2 GetPosition() const { return glm::vec2(m_Position.x, m_Position.y); }
	glm::vec2 GetCenter() const { return m_Center; }
	float GetRotation() const { return m_Rotation; }
	float GetZoom() const { return m_Zoom; }
	float GetAspectRatio() const { return m_AspectRatio; }

	static Camera* GetActiveCamera() { return m_CurrActiveCamera; }
	static void ClearActiveCamera() { m_CurrActiveCamera = nullptr; }

	///
	// Unique Movement
	///

	void Orbit(float degrees, glm::vec3 axis); // Rotates around target
	void OrbitAround(glm::vec3 center, float degrees, glm::vec3 axis); // Rotates around center

private:
	friend void ImGui_Camera(Camera *camera, GameObject object, Editor *editor);
	friend glm::vec2 Input::ScreenToWorld(glm::vec2 cursor);

	void ApplyCameraMatrices();

	// Private getters for meta purposes.
	float GetNearPlane() const { return m_Near; }
	float GetFarPlane() const { return m_Far; }

	static Camera* m_CurrActiveCamera;
	
	// Uniform buffer object location
	static GLuint m_MatricesUbo;

	//View matrix
	glm::vec3 m_Position = glm::vec3(0, 0, 5.0f);
	glm::vec3 m_Center = glm::vec3(0, 0, 0);
	glm::vec3 m_Up = glm::vec3(0, 1, 0);

	float m_Rotation = 0;

	//Projection matrix
	float m_Zoom = 3.0f;
	float m_AspectRatio;
	float m_Near = 1.0f;
	float m_Far = 10.0f;

	void SetIsActiveCam(bool isActive)
	{
		if (isActive)
		{
			this->Use();
		}
		else
		{
			// Don't bother doing anything if we set this to false.
		}
	}

	META_REGISTER(Camera)
	{
		META_DefineGetterSetter(Camera, glm::vec2, GetPosition, SetPosition, "position");

		META_DefineGetterSetter(Camera, float, GetZoom, SetZoom, "zoom");
		META_DefineGetterSetter(Camera, float, GetAspectRatio, SetAspectRatio, "aspectRatio");
		//META_DefineGetterSetter(Camera, float, GetNearPlane, SetNearPlane, "nearPlane");
		//META_DefineGetterSetter(Camera, float, GetFarPlane, SetFarPlane, "farPlane");

		META_DefineGetterSetter(Camera, bool, IsActiveCam, SetIsActiveCam, "isActiveCam");

	}
};

