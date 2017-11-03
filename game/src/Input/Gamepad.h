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
    int ID_;                                  // Gamepad ID
    bool enabled_;                            // Gamepad is connected
    int numAxes_;                             // Number of axes in gamepad
    int numButtons_;                          // Number of buttons in gamepad
    float const * axisValues_;                // 6 axes: left analog (x,y), right analog (x,y), L2 (sensitivity), R2 (sensitivity))
    unsigned char * buttonsPrevState_;        // Store previous button state
    unsigned char * buttonsCurrState_;        //
    unsigned char const * buttonsNextState_;  // Provided by GLFW

    // Include dead zone

    void GamepadDebug();
    void BitPrint(const unsigned char * const array, int count);

  public:
    Gamepad(int ID);
    ~Gamepad();
    void Update();

    void UpdateButtons();
    void UpdateAxes();

    void EnableGamepad();
    void DisableGamepad();
    int GetGamepadID();
    int GetGamepadButton();
    float GetGamepadAxis(GamepadAxis axis);

    bool IsConnected();
    bool IsPressed(GamepadButton button);
    bool IsHeldDown(GamepadButton button);
    bool IsReleased(GamepadButton button);
};
