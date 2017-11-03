/*
FILE: RenderSystem.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObjectSystem\GameSpace.h"

struct GLFWwindow;

class RenderSystem : public SystemBase
{
public:
	RenderSystem();

	virtual void Init();

	// Called each frame.
	virtual void Update(float dt);

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority();

	static void ResizeWindowEvent(GLFWwindow* window, int width, int height);

	// Inherited via SystemBase
	virtual SystemBase * NewDuplicate() override;
private:
	void UpdateCameras(float dt);
	void RenderSprites(float dt);
	void RenderText(float dt);
	void RenderParticles(float dt);
	void RenderBackgrounds(float dt);
	void RenderForegrounds(float dt);
};
