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
    int ID_;       // gamepad ID
    bool enabled_; // gamepad Enabled
    int axes_;
    int buttons_;
    std::map<int, std::pair<int, bool>> gamepadButtonEvents_;  // Button state (pressed, held down, released), EventHandled
    std::map<int, float> gamepadAxisVals_;                            // Axis values (-1 to 1)
    // Include dead zone

    // Current button states
    unsigned char * buttonsPrevState_;
    unsigned char const * buttonsCurrState_;

    float const * axesCurrState_; // 6 axes: left analog (x,y), right analog (x,y), L2 (sensitivity), R2 (sensitivity))

  public:
    Gamepad(int ID);
    ~Gamepad();
    void Update();

    void UpdateButtons();
    void UpdateAxes();

    void SetGamepadButton();
    void SetGamepadAxis();
    void EnableGamepad();
    void DisableGamepad();
    int GetGamepadID();
    int GetGamepadButton();
    int GetGamepadAxis();

    bool IsConnected();
    bool IsPressed(int button);
    bool IsHeldDown(int button);
    bool IsReleased(int button);
    
    glm::vec2 GetGamepadAxes(int axes);
    float GetGamepadSingleAxis(int axis);

    void GamepadDebug();
};
