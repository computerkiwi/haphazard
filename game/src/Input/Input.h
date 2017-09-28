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
#include "Controls.h"

#include <map>
#include <array>

// Key binding
/*class Controls
{
  public:
    // Default configuration constructor
    Controls();

    Controls const & GetConfig() const;

  private:
    int jump_;
    int moveLeft_;
    int moveRight_;
    int attack_;
};

typedef enum CONTROLS
{

  MOVE_LEFT = GLFW_KEY_A,
  MOVE_RIGHT = GLFW_KEY_D

}CONTROLS;
*/


namespace Input
{
    ////////// Functions //////////
    void Init(GLFWwindow * window);
    void Update();
    void Exit();

    bool IsPressed(KeyboardMap key);
    bool IsHeldDown(KeyboardMap key);

    void SetControls();
    void Input_Debug(KeyboardMap key);

    ////////// Callback Functions //////////
    // GLFW doesn't know what an object is, needs static
    static void KeyCallback(GLFWwindow * window, int key, int scancode, int actions, int mods);
    static void CursorCallback(GLFWwindow * window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod);
    static void ControllerCallback(int joy, int event);

    // Group them per controller
    // 2 floats (for joysticks)
    // 13 buttons (4 trigger buttons, letter buttons, arrow button stuff, joystick button??)

};

// For key binding: use .txt file (or something) to save mapping