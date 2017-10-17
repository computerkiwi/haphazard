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

// Key binding
/*class Gamepad
{
  public:
    // Default configuration constructor
    Gamepad();

    Gamepad const & GetConfig() const;

  private:
    int jump_;
    int moveLeft_;
    int moveRight_;
    int attack_;
};

typedef enum Gamepad
{

  MOVE_LEFT = GLFW_KEY_A,
  MOVE_RIGHT = GLFW_KEY_D

}Gamepad;
*/


namespace Input
{
    ////////// Functions //////////
    void Init(GLFWwindow * window);
    void Update();
    void Exit();

    // Button functions
    bool IsPressed(Key key);
    bool IsHeldDown(Key key);

    // Cursor functions
    glm::vec2 GetMousePos();
    glm::vec2 ScreenToWorld(glm::vec2 screen);

    // !!! Not yet implemented
    void SetGamepad();
    void Input_Debug(Key key);

};

// For key binding: use .txt file (or something) to save mapping