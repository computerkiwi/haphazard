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

    typedef std::map< int, TRIGGER_STATE> TriggerMap;

    ////////// Static Variables //////////
    static GLFWwindow * inputWindow;    // Window to detect input from
    static TriggerMap triggerMap;       // 0 to 9, A to Z, special characters, modifiers, numpad
    static glm::vec2 cursorPos;         // x, y cursor positions; top-left is origin

    ////////// Functions        //////////
    void Init(GLFWwindow * window);
    void Update();
    void Exit();

    void SetControls();
    void KeyCheck_Debug(int key);

    // GLFW doesn't know what an object is, needs static
    static void KeyCallback(GLFWwindow * window, int key, int scancode, int actions, int mods);
    static void CursorCallback(GLFWwindow * window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mod);
};

// For key binding: use .txt file (or something) to save mapping