/*******************************************************
FILE: Gamepad.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Gamepad.h"

Gamepad::Gamepad(int ID)
  : ID_(ID)
  , enabled_(false)
  , axesCurrState_(NULL)
  , buttonsCurrState_(NULL)
  , buttonsPrevState_(NULL)
{
}

Gamepad::~Gamepad()
{
  // oh my god what am I doing
  delete buttonsPrevState_;
}

void Gamepad::SetGamepadButton()
{
}

void Gamepad::SetGamepadAxis()
{
}

// ID of the gamepad
int Gamepad::GetGamepadID()
{
  return ID_;
}

int Gamepad::GetGamepadButton()
{
  return buttons_;
}

int Gamepad::GetGamepadAxis()
{
  return axes_;
}


void Gamepad::Update()
{
  // Do nothing if gamepad disabled
  if (enabled_ == false)
  {
    return;
  }

  UpdateAxes();
  UpdateButtons();
}

// Retrieves current state of axes and stores values in gamepadAxisVals_
void Gamepad::UpdateAxes()
{
  // Retrieve current state
  axesCurrState_ = glfwGetJoystickAxes(ID_, &axes_);

  //=========================================
  // Left Analog Axes
  //=========================================
  if (axesCurrState_[GamepadAxis::LeftAnalog_X] != 0)
  {
    gamepadAxisVals_[GamepadAxis::LeftAnalog_X] = axesCurrState_[GamepadAxis::LeftAnalog_X];
    std::cout << "Left analog X axis: " << gamepadAxisVals_[GamepadAxis::LeftAnalog_X] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::LeftAnalog_Y] != 0)
  {
    gamepadAxisVals_[GamepadAxis::LeftAnalog_Y] = axesCurrState_[GamepadAxis::LeftAnalog_Y];
    std::cout << "Left analog Y axis: " << gamepadAxisVals_[GamepadAxis::LeftAnalog_Y] << std::endl;
  }

  //=========================================
  //  Right Analog Axes
  //=========================================
  if (axesCurrState_[GamepadAxis::RightAnalog_X] != 0)
  {
    gamepadAxisVals_[GamepadAxis::RightAnalog_X] = axesCurrState_[GamepadAxis::RightAnalog_X];
    std::cout << "Right analog X axis: " << gamepadAxisVals_[GamepadAxis::RightAnalog_X] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::RightAnalog_Y] != 0)
  {
    gamepadAxisVals_[GamepadAxis::RightAnalog_Y] = axesCurrState_[GamepadAxis::RightAnalog_Y];
    std::cout << "Right Y axis: " << gamepadAxisVals_[GamepadAxis::RightAnalog_Y] << std::endl;
  }

  //=========================================
  //  Trigger Axes measures sensitivity
  //=========================================
  if (axesCurrState_[GamepadAxis::LeftTrigger] > -1)
  {
    gamepadAxisVals_[GamepadAxis::LeftTrigger] = axesCurrState_[GamepadAxis::LeftTrigger];
    std::cout << "Left Trugger/L2: " << gamepadAxisVals_[GamepadAxis::LeftTrigger] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::RightTrigger] > -1)
  {
    gamepadAxisVals_[GamepadAxis::RightTrigger] = axesCurrState_[GamepadAxis::RightTrigger];
    std::cout << "Right Trigger/R2: " << gamepadAxisVals_[GamepadAxis::RightTrigger] << std::endl;
  }
}

void Gamepad::UpdateButtons()
{
  // Save previous button state
  *buttonsPrevState_ = *buttonsCurrState_;

  // Get current button state
  buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);

  // Use bit shift

  //*buttonsPrevState_ = '0';

  //for (int i = 0; i < sizeof(unsigned char); ++i)
  //{
  //    // Button pressed
  //    if (buttonsCurrState_[i] > buttonsPrevState_[i])
  //    {
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::Pressed;
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //    }
  //    // Held down
  //    else if ((buttonsCurrState_[i] == buttonsPrevState_[i]) && (buttonsCurrState_[i] > 0))
  //    {
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::HeldDown;
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //    }
  //    // Button released
  //    else if (buttonsCurrState_[i] < buttonsPrevState_[i])
  //    {
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::Released;
  //      gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //    }
  //}

  //// Save previous state
  //*buttonsPrevState_ = *buttonsCurrState_;

  //// Retrieve current state
  //buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);

  //// Loop through button state array
  //for (int i = 0; i < sizeof(buttonsCurrState_); ++i)
  //{
  //  // Button pressed
  //  if (buttonsCurrState_[i] > buttonsPrevState_[i])
  //  {
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::Pressed;
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //  }
  //  // Button held down
  //  else if ((buttonsCurrState_[i] == buttonsPrevState_[i]) && (buttonsCurrState_[i] > 0))
  //  {
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::HeldDown;
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //  }
  //  // Button released
  //  else if (buttonsCurrState_[i] < buttonsPrevState_[i])
  //  {
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::Released;
  //    gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = false;
  //  }
    //// State hasn't changed
    //else
    //{
    //  gamepadButtonEvents_[static_cast<GamepadButton>(i)].first = KeyState::Released;
    //  gamepadButtonEvents_[static_cast<GamepadButton>(i)].second = true;
    //}
  //}
}

bool Gamepad::IsPressed(int button)
{
  if ((gamepadButtonEvents_[button].first == KeyState::Pressed) && (gamepadButtonEvents_[button].second == false))
  {
    gamepadButtonEvents_[button].second = true;
    return true;
  }
  return false;
}

bool Gamepad::IsHeldDown(int button)
{
  if ((gamepadButtonEvents_[button].first == KeyState::HeldDown) && (gamepadButtonEvents_[button].second == false))
  {
    gamepadButtonEvents_[button].second = true;
    return true;
  }
  return false;
}

bool Gamepad::IsReleased(int button)
{
  if ((gamepadButtonEvents_[button].first == KeyState::Released) && (gamepadButtonEvents_[button].second == false))
  {
    gamepadButtonEvents_[button].second = true;
    return true;
  }
  return false;
}

float Gamepad::GetGamepadSingleAxis(int axis)
{
  return axesCurrState_[axis];
}

bool Gamepad::IsConnected()
{
  return enabled_;
}

void Gamepad::EnableGamepad()
{
  buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);
  axesCurrState_ = glfwGetJoystickAxes(ID_, &axes_);
  enabled_ = true;
}

void Gamepad::DisableGamepad()
{
  enabled_ = false;
}

void Gamepad::GamepadDebug()
{
  // ???? Sometimes this is a NullPtr?
  // Retrieve current state
  axesCurrState_ = glfwGetJoystickAxes(ID_, &axes_);
  buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);

  if (axesCurrState_ == NULL || buttonsCurrState_ == NULL)
  {
    return;
  }

  //if (enabled_ == false)
  //{
  //  return;
  //}

  ////// Axes /////////////////////////////
  // Left Analog Axes
  if (axesCurrState_[GamepadAxis::LeftAnalog_X] != 0)
  {
    std::cout << "Left analog X axis: " << axesCurrState_[0] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::LeftAnalog_Y] != 0)
  {
    std::cout << "Left analog Y axis: " << axesCurrState_[1] << std::endl;
  }
  // Right Analog Axes
  if (axesCurrState_[GamepadAxis::RightAnalog_X] != 0)
  {
    std::cout << "Right analog X axis: " << axesCurrState_[2] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::RightAnalog_Y] != 0)
  {
    std::cout << "Right Y axis: " << axesCurrState_[3] << std::endl;
  }
  // Trigger Axes; measures sensitivity
  if (axesCurrState_[GamepadAxis::LeftTrigger] > -1)
  {
    std::cout << "Left Trugger/L2: " << axesCurrState_[4] << std::endl;
  }
  if (axesCurrState_[GamepadAxis::RightTrigger] > -1)
  {
    std::cout << "Right Trigger/R2: " << axesCurrState_[5] << std::endl;
  }

  ////// Buttons /////////////////////////////
  // 'A' Button
  if (buttonsCurrState_[GamepadButton::A] == KeyState::Pressed)
  {
    std::cout << "Button A is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::A] == KeyState::HeldDown)
  {
    std::cout << "Button A is held down" << std::endl;
  }

  // 'B' Button
  if (buttonsCurrState_[GamepadButton::B] == KeyState::Pressed)
  {
    std::cout << "Button B is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::B] == KeyState::HeldDown)
  {
    std::cout << "Button B is held down" << std::endl;
  }

  // 'X' Button
  if (buttonsCurrState_[GamepadButton::X] == KeyState::Pressed)
  {
    std::cout << "Button X is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::X] == KeyState::HeldDown)
  {
    std::cout << "Button X is held down" << std::endl;
  }

  // 'Y' Button
  if (buttonsCurrState_[GamepadButton::Y] == KeyState::Pressed)
  {
    std::cout << "Button Y is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Y] == KeyState::HeldDown)
  {
    std::cout << "Button Y is held down" << std::endl;
  }

  // Left Trigger
  if (buttonsCurrState_[GamepadButton::LeftTrigger] == KeyState::Pressed)
  {
    std::cout << "Left Trigger is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::LeftTrigger] == KeyState::HeldDown)
  {
    std::cout << "Left Trigger is held down" << std::endl;
  }

  // Right Trigger
  if (buttonsCurrState_[GamepadButton::RightTrigger] == KeyState::Pressed)
  {
    std::cout << "Right Trigger is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::RightTrigger] == KeyState::HeldDown)
  {
    std::cout << "Right Trigger is held down" << std::endl;
  }

  // Back button
  if (buttonsCurrState_[GamepadButton::Back] == KeyState::Pressed)
  {
    std::cout << "Back button is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Back] == KeyState::HeldDown)
  {
    std::cout << "Back button is held down" << std::endl;
  }

  // Start Button
  if (buttonsCurrState_[GamepadButton::Start] == KeyState::Pressed)
  {
    std::cout << "Start button is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Start] == KeyState::HeldDown)
  {
    std::cout << "Start button is held down" << std::endl;
  }

  //// Guide Button
  //if (buttonsCurrState_[8] == KeyState::Pressed))
  //{
  //  std::cout << "Guide button is pressed" << std::endl;
  //}
  //else if (buttonsCurrState_[8] == KeyState::HeldDown))
  //{
  //  std::cout << "Guide button is held down" << std::endl;
  //}

  // Left Thumb
  if (buttonsCurrState_[GamepadButton::LeftThumb] == KeyState::Pressed)
  {
    std::cout << "Left Thumb is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::LeftThumb] == KeyState::HeldDown)
  {
    std::cout << "Left Thumb is held down" << std::endl;
  }

  // Right Thumb
  if (buttonsCurrState_[GamepadButton::RightThumb] == KeyState::Pressed)
  {
    std::cout << "Right Thumb is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::RightThumb] == KeyState::HeldDown)
  {
    std::cout << "Right Thumb is held down" << std::endl;
  }

  // Directional Pad Up
  if (buttonsCurrState_[GamepadButton::Up] == KeyState::Pressed)
  {
    std::cout << "DPad UP is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Up] == KeyState::HeldDown)
  {
    std::cout << "DPad UP is held down" << std::endl;
  }

  // Directional Pad Right
  if (buttonsCurrState_[GamepadButton::Right] == KeyState::Pressed)
  {
    std::cout << "DPad RIGHT is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Right] == KeyState::HeldDown)
  {
    std::cout << "DPad RIGHT is held down" << std::endl;
  }

  // Directional Pad Down
  if (buttonsCurrState_[GamepadButton::Down] == KeyState::Pressed)
  {
    std::cout << "DPad DOWN is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Down] == KeyState::HeldDown)
  {
    std::cout << "DPad DOWN is held down" << std::endl;
  }

  // Directional Pad Left
  if (buttonsCurrState_[GamepadButton::Left] == KeyState::Pressed)
  {
    std::cout << "DPad LEFT is pressed" << std::endl;
  }
  else if (buttonsCurrState_[GamepadButton::Left] == KeyState::HeldDown)
  {
    std::cout << "DPad LET is held down" << std::endl;
  }
}
/*
glm::vec2 GetGamepadAxes(GamepadAxis axes)
{
  // Convert int axes to vec2

  return glm::vec2(;
}*/
