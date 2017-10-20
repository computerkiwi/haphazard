/*******************************************************
FILE: Gamepad.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Gamepad.h"

Gamepad::Gamepad(int ID) : ID_(ID)
{
  axesCurrState_ = glfwGetJoystickAxes(ID_, &axes_);
  buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);
}

Gamepad::~Gamepad()
{
}

void Gamepad::SetGamepadButton(unsigned char const & buttons)
{
  buttons_ = buttons;
}

void Gamepad::SetGamepadAxis(float const & axes)
{
  axes_ = (int)axes;
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

bool Gamepad::IsPressed(GamepadButton button)
{
  if (buttonsCurrState_[static_cast<int>(button)] == static_cast<int>(KeyState::Pressed))
  {
    return true;
  }
  return false;
}

bool Gamepad::IsHeldDown(GamepadButton button)
{
  if (buttonsCurrState_[static_cast<int>(button)] == static_cast<int>(KeyState::HeldDown))
  {
    return true;
  }
  return false;
}

float Gamepad::GetGamepadSingleAxis(GamepadAxis axis)
{
  return axesCurrState_[static_cast<int>(axis)];
}


void Gamepad::GamepadDebug()
{
  // ???? Sometimes this is a NullPtr?
  // Retrieve current state
  axesCurrState_ = glfwGetJoystickAxes(ID_, &axes_);
  buttonsCurrState_ = glfwGetJoystickButtons(ID_, &buttons_);

  ////// Axes /////////////////////////////
  // Left Analog Axes
  if (axesCurrState_[static_cast<int>(GamepadAxis::LeftAnalog_X)] != 0)
  {
    std::cout << "Left analog X axis: " << axesCurrState_[0] << std::endl;
  }
  if (axesCurrState_[static_cast<int>(GamepadAxis::LeftAnalog_Y)] != 0)
  {
    std::cout << "Left analog Y axis: " << axesCurrState_[1] << std::endl;
  }
  // Right Analog Axes
  if (axesCurrState_[static_cast<int>(GamepadAxis::RightAnalog_X)] != 0)
  {
    std::cout << "Right analog X axis: " << axesCurrState_[2] << std::endl;
  }
  if (axesCurrState_[static_cast<int>(GamepadAxis::RightAnalog_Y)] != 0)
  {
    std::cout << "Right Y axis: " << axesCurrState_[3] << std::endl;
  }
  // Trigger Axes; measures sensitivity
  if (axesCurrState_[static_cast<int>(GamepadAxis::LeftTrigger)] > -1)
  {
    std::cout << "Left Trugger/L2: " << axesCurrState_[4] << std::endl;
  }
  if (axesCurrState_[static_cast<int>(GamepadAxis::RightTrigger)] > -1)
  {
    std::cout << "Right Trigger/R2: " << axesCurrState_[5] << std::endl;
  }

  ////// Buttons /////////////////////////////
  // 'A' Button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::A)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Button A is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::A)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Button A is held down" << std::endl;
  }

  // 'B' Button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::B)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Button B is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::B)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Button B is held down" << std::endl;
  }

  // 'X' Button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::X)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Button X is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::X)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Button X is held down" << std::endl;
  }

  // 'Y' Button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Y)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Button Y is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Y)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Button Y is held down" << std::endl;
  }

  // Left Trigger
  if (buttonsCurrState_[static_cast<int>(GamepadButton::LeftTrigger)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Left Trigger is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::LeftTrigger)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Left Trigger is held down" << std::endl;
  }

  // Right Trigger
  if (buttonsCurrState_[static_cast<int>(GamepadButton::RightTrigger)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Right Trigger is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::RightTrigger)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Right Trigger is held down" << std::endl;
  }

  // Back button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Back)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Back button is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Back)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Back button is held down" << std::endl;
  }

  // Start Button
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Start)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Start button is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Start)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Start button is held down" << std::endl;
  }

  //// Guide Button
  //if (buttonsCurrState_[8] == static_cast<int>(KeyState::Pressed))
  //{
  //  std::cout << "Guide button is pressed" << std::endl;
  //}
  //else if (buttonsCurrState_[8] == static_cast<int>(KeyState::HeldDown))
  //{
  //  std::cout << "Guide button is held down" << std::endl;
  //}

  // Left Thumb
  if (buttonsCurrState_[static_cast<int>(GamepadButton::LeftThumb)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Left Thumb is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::LeftThumb)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Left Thumb is held down" << std::endl;
  }

  // Right Thumb
  if (buttonsCurrState_[static_cast<int>(GamepadButton::RightThumb)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "Right Thumb is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::RightThumb)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "Right Thumb is held down" << std::endl;
  }

  // Directional Pad Up
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Up)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "DPad UP is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Up)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "DPad UP is held down" << std::endl;
  }

  // Directional Pad Right
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Right)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "DPad RIGHT is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Right)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "DPad RIGHT is held down" << std::endl;
  }

  // Directional Pad Down
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Down)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "DPad DOWN is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Down)] == static_cast<int>(KeyState::HeldDown))
  {
    std::cout << "DPad DOWN is held down" << std::endl;
  }

  // Directional Pad Left
  if (buttonsCurrState_[static_cast<int>(GamepadButton::Left)] == static_cast<int>(KeyState::Pressed))
  {
    std::cout << "DPad LEFT is pressed" << std::endl;
  }
  else if (buttonsCurrState_[static_cast<int>(GamepadButton::Left)] == static_cast<int>(KeyState::HeldDown))
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
