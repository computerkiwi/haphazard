/*******************************************************
FILE: Input.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "GLFW\glfw3.h"
#include "glm\vec2.hpp"
#include "KeyMap.h"
#include "Gamepad.h"

#include <map>
#include <vector>
#include <iterator>

namespace Input
{
    ////////// Main Input Functions //////////
    void Init(GLFWwindow * window);
    void Update();
    void Exit();

    // Key states
    bool IsPressed(Key key);
    bool IsHeldDown(Key key);
    bool IsReleased(Key key);
    void SetKeyState(int key, KeyState state);

    void InputDebug(Key key1, Key key2, Key key3);

    // Gamepad functions
    void SetGamepad();
    float GamepadGetAxis(PlayerID player, GamepadAxis axis);
    bool GamepadIsPressed(PlayerID player, GamepadButton button);
    bool GamepadIsHeldDown(PlayerID player, GamepadButton button);
    bool GamepadIsReleased(PlayerID player, GamepadButton button);

    // Update functions
    void UpdateKeyStates();
    void UpdateGamepads();

    // Cursor functions
    glm::vec2 GetMousePos();
    glm::vec2 GetMousePos_World();
	void RecordMousePos();
	glm::vec2 GetPrevMousePos();
	glm::vec2 GetPrevMousePos_World();
    glm::vec2 ScreenToWorld(glm::vec2 cursor);

    ////////// Callback Functions //////////
    // GLFW doesn't know what an object is, needs static
    // Functions set once
    static void KeyCallback(GLFWwindow * window, int key, int scancode, int actions, int mods);
    static void CursorCallback(GLFWwindow * window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod);
    static void GamepadCallback(int joy, int event);
};
