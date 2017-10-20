/*******************************************************
FILE: Gamepad.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "GLFW\glfw3.h"
#include "glm\vec2.hpp"
#include "KeyMap.h"

#include <iostream>
#include <map>

//typedef std::map<GamepadMap, KeyState> Gamepad;

class Gamepad
{


  private:
    int ID_;
    int buttons_;
    int axes_; // 6 axes: left analog (x,y), right analog (x,y), L2 (sensitivity), R2 (sensitivity))
    unsigned char const * buttonsCurrState_;
    float const * axesCurrState_;

  public:
    Gamepad(int ID);
    ~Gamepad();

    void SetGamepadButton(unsigned char const & buttons);
    void SetGamepadAxis(float const & axes);
    int GetGamepadID();
    int GetGamepadButton();
    int GetGamepadAxis();

    bool IsPressed(GamepadButton button);
    bool IsHeldDown(GamepadButton button);
    
    glm::vec2 GetGamepadAxes(GamepadAxis axes);
    float GetGamepadSingleAxis(GamepadAxis axis);

    void GamepadDebug();
};
