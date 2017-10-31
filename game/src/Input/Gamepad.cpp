/*******************************************************
FILE: Gamepad.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include <cstring>
#include "Gamepad.h"

Gamepad::Gamepad(int ID)
  : ID_(ID)
  , enabled_(false)
  , numAxes_(0)
  , numButtons_(0)
  , axisValues_(NULL)
  , buttonsPrevState_(NULL)
  , buttonsCurrState_(NULL)
  , buttonsNextState_(NULL)
{
}

Gamepad::~Gamepad()
{
  DisableGamepad();
}

// ID of the gamepad
int Gamepad::GetGamepadID()
{
  return ID_;
}

// Number of buttons on gamepad
int Gamepad::GetGamepadButton()
{
  return numButtons_;
}

// Get value of specific axis
float Gamepad::GetGamepadAxis(GamepadAxis axis)
{
  return axisValues_[static_cast<int>(axis)];
}

void Gamepad::Update()
{
  // Do nothing if gamepad disabled
  if (enabled_ == false)
  {
    return;
  }

//  UpdateAxes();
  UpdateButtons();
}

//// Retrieves current state of axes and stores values in gamepadAxisVals_
//void Gamepad::UpdateAxes()
//{
//  //=========================================
//  // Left Analog Axes
//  //=========================================
//  if (axisValues_[GamepadAxis::LeftAnalog_X] != 0)
//  {
//    gamepadAxisVals_[GamepadAxis::LeftAnalog_X] = axisValues_[GamepadAxis::LeftAnalog_X];
//    std::cout << "Left analog X axis: " << gamepadAxisVals_[GamepadAxis::LeftAnalog_X] << std::endl;
//  }
//  if (axisValues_[GamepadAxis::LeftAnalog_Y] != 0)
//  {
//    gamepadAxisVals_[GamepadAxis::LeftAnalog_Y] = axisValues_[GamepadAxis::LeftAnalog_Y];
//    std::cout << "Left analog Y axis: " << gamepadAxisVals_[GamepadAxis::LeftAnalog_Y] << std::endl;
//  }
//
//  //=========================================
//  //  Right Analog Axes
//  //=========================================
//  if (axisValues_[GamepadAxis::RightAnalog_X] != 0)
//  {
//    gamepadAxisVals_[GamepadAxis::RightAnalog_X] = axisValues_[GamepadAxis::RightAnalog_X];
//    std::cout << "Right analog X axis: " << gamepadAxisVals_[GamepadAxis::RightAnalog_X] << std::endl;
//  }
//  if (axisValues_[GamepadAxis::RightAnalog_Y] != 0)
//  {
//    gamepadAxisVals_[GamepadAxis::RightAnalog_Y] = axisValues_[GamepadAxis::RightAnalog_Y];
//    std::cout << "Right Y axis: " << gamepadAxisVals_[GamepadAxis::RightAnalog_Y] << std::endl;
//  }
//
//  //=========================================
//  //  Trigger Axes (sensitivity)
//  //=========================================
//  if (axisValues_[GamepadAxis::LeftTrigger] > -1)
//  {
//    gamepadAxisVals_[GamepadAxis::LeftTrigger] = axisValues_[GamepadAxis::LeftTrigger];
//    std::cout << "Left Trugger/L2: " << gamepadAxisVals_[GamepadAxis::LeftTrigger] << std::endl;
//  }
//  if (axisValues_[GamepadAxis::RightTrigger] > -1)
//  {
//    gamepadAxisVals_[GamepadAxis::RightTrigger] = axisValues_[GamepadAxis::RightTrigger];
//    std::cout << "Right Trigger/R2: " << gamepadAxisVals_[GamepadAxis::RightTrigger] << std::endl;
//  }
//}

void Gamepad::UpdateButtons()
{
  // Both previous and current button states exist
  if (buttonsPrevState_ && buttonsCurrState_)
  {
    // Copy over current button states
    memcpy(buttonsPrevState_, buttonsCurrState_, numButtons_);
  }

  //std::cout << "Prev state: " << std::endl;
  //BitPrint(buttonsPrevState_, numButtons_);

  //std::cout << "Curr state: " << std::endl;;
  //BitPrint(buttonsCurrState_, numButtons_);

  //std::cout << "Next state: " << std::endl;
  //BitPrint(buttonsNextState_, numButtons_);

  // Current button state exists
  if (buttonsNextState_ != NULL)
  {
    // Copy over next button states
    memcpy(buttonsCurrState_, buttonsNextState_, numButtons_);
  }

  //std::cout << "NEW Curr state: " << std::endl;
  //BitPrint(buttonsCurrState_, numButtons_);

}

bool Gamepad::IsPressed(GamepadButton gamepadbutton)
{
  int button = static_cast<int>(gamepadbutton);

  if ((buttonsPrevState_[button] == KeyState::Released) &&
      (buttonsCurrState_[button] == KeyState::Pressed))
  {
    //std::cout << "Is Pressed" << std::endl;
    //std::cout << "Prev state: ";
    //BitPrint(buttonsPrevState_, numButtons_);

    //std::cout << "Curr state: ";
    //BitPrint(buttonsCurrState_, numButtons_);

    return true;
  }

  return false;
}

bool Gamepad::IsHeldDown(GamepadButton gamepadbutton)
{
  int button = static_cast<int>(gamepadbutton);

  if (buttonsCurrState_[button] == KeyState::Pressed)
  {
    //std::cout << "Is HeldDown" << std::endl;
    //std::cout << "Prev state: ";
    //BitPrint(buttonsPrevState_, numButtons_);

    //std::cout << "Curr state: ";
    //BitPrint(buttonsCurrState_, numButtons_);

    return true;
  }
  return false;
}

bool Gamepad::IsReleased(GamepadButton gamepadbutton)
{
  int button = static_cast<int>(gamepadbutton);

  if ((buttonsPrevState_[button] == KeyState::Pressed) &&
      (buttonsCurrState_[button] == KeyState::Released))
  {
    //std::cout << "Is Released" << std::endl;
    //std::cout << "Prev state: ";
    //BitPrint(buttonsPrevState_, numButtons_);

    //std::cout << "Curr state: ";
    //BitPrint(buttonsCurrState_, numButtons_);

    return true;
  }
  return false;
}

// Checks if gamepad is connected
bool Gamepad::IsConnected()
{
  return enabled_;
}

void Gamepad::EnableGamepad()
{
  // Connect with GLFW library
  buttonsNextState_ = glfwGetJoystickButtons(ID_, &numButtons_);
  axisValues_ = glfwGetJoystickAxes(ID_, &numAxes_);

  // Next state successful
  if (buttonsNextState_ != NULL)
  {
    // Allocate memory for previous state
    buttonsPrevState_ = new unsigned char[numButtons_];
    memset(buttonsPrevState_, 0, numButtons_);

    // Allocate memory for current state and copy next state
    buttonsCurrState_ = new unsigned char[numButtons_];
    memcpy(buttonsCurrState_, buttonsNextState_, numButtons_);
  }

  // Enable gamepad
  enabled_ = true;
}

void Gamepad::DisableGamepad()
{
  // Deallocate memory
  if (buttonsPrevState_)
  {
    delete buttonsPrevState_;
  }
  if (buttonsCurrState_)
  {
    delete buttonsCurrState_;
  }

  // Disable gamepad
  enabled_ = false;
}

//void Gamepad::GamepadDebug()
//{
//  // ???? Sometimes this is a NullPtr?
//  // Retrieve current state
//  axisValues_ = glfwGetJoystickAxes(ID_, &numAxes_);
//  buttonsNextState_ = glfwGetJoystickButtons(ID_, &numButtons_);
//
//  if (axisValues_ == NULL || buttonsCurrState_ == NULL)
//  {
//    return;
//  }
//
//  //if (enabled_ == false)
//  //{
//  //  return;
//  //}
//
//  ////// Axes /////////////////////////////
//  // Left Analog Axes
//  if (axisValues_[static_cast<int>(GamepadAxis::LeftAnalog_X)] != 0)
//  {
//    std::cout << "Left analog X axis: " << axisValues_[0] << std::endl;
//  }
//  if (axisValues_[static_cast<int>(GamepadAxis::LeftAnalog_Y)] != 0)
//  {
//    std::cout << "Left analog Y axis: " << axisValues_[1] << std::endl;
//  }
//  // Right Analog Axes
//  if (axisValues_[static_cast<int>(GamepadAxis::RightAnalog_X)] != 0)
//  {
//    std::cout << "Right analog X axis: " << axisValues_[2] << std::endl;
//  }
//  if (axisValues_[static_cast<int>(GamepadAxis::RightAnalog_Y)] != 0)
//  {
//    std::cout << "Right Y axis: " << axisValues_[3] << std::endl;
//  }
//  // Trigger Axes; measures sensitivity
//  if (axisValues_[static_cast<int>(GamepadAxis::LeftTrigger)] > -1)
//  {
//    std::cout << "Left Trugger/L2: " << axisValues_[4] << std::endl;
//  }
//  if (axisValues_[static_cast<int>(GamepadAxis::RightTrigger)] > -1)
//  {
//    std::cout << "Right Trigger/R2: " << axisValues_[5] << std::endl;
//  }
//
//  ////// Buttons /////////////////////////////
//  // 'A' Button
//  if (buttonsCurrState_[GamepadButton::A] == KeyState::Pressed)
//  {
//    std::cout << "Button A is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::A] == KeyState::HeldDown)
//  {
//    std::cout << "Button A is held down" << std::endl;
//  }
//
//  // 'B' Button
//  if (buttonsCurrState_[GamepadButton::B] == KeyState::Pressed)
//  {
//    std::cout << "Button B is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::B] == KeyState::HeldDown)
//  {
//    std::cout << "Button B is held down" << std::endl;
//  }
//
//  // 'X' Button
//  if (buttonsCurrState_[GamepadButton::X] == KeyState::Pressed)
//  {
//    std::cout << "Button X is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::X] == KeyState::HeldDown)
//  {
//    std::cout << "Button X is held down" << std::endl;
//  }
//
//  // 'Y' Button
//  if (buttonsCurrState_[GamepadButton::Y] == KeyState::Pressed)
//  {
//    std::cout << "Button Y is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Y] == KeyState::HeldDown)
//  {
//    std::cout << "Button Y is held down" << std::endl;
//  }
//
//  // Left Trigger
//  if (buttonsCurrState_[GamepadButton::LeftTrigger] == KeyState::Pressed)
//  {
//    std::cout << "Left Trigger is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::LeftTrigger] == KeyState::HeldDown)
//  {
//    std::cout << "Left Trigger is held down" << std::endl;
//  }
//
//  // Right Trigger
//  if (buttonsCurrState_[GamepadButton::RightTrigger] == KeyState::Pressed)
//  {
//    std::cout << "Right Trigger is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::RightTrigger] == KeyState::HeldDown)
//  {
//    std::cout << "Right Trigger is held down" << std::endl;
//  }
//
//  // Back button
//  if (buttonsCurrState_[GamepadButton::Back] == KeyState::Pressed)
//  {
//    std::cout << "Back button is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Back] == KeyState::HeldDown)
//  {
//    std::cout << "Back button is held down" << std::endl;
//  }
//
//  // Start Button
//  if (buttonsCurrState_[GamepadButton::Start] == KeyState::Pressed)
//  {
//    std::cout << "Start button is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Start] == KeyState::HeldDown)
//  {
//    std::cout << "Start button is held down" << std::endl;
//  }
//
//  //// Guide Button
//  //if (buttonsCurrState_[8] == KeyState::Pressed))
//  //{
//  //  std::cout << "Guide button is pressed" << std::endl;
//  //}
//  //else if (buttonsCurrState_[8] == KeyState::HeldDown))
//  //{
//  //  std::cout << "Guide button is held down" << std::endl;
//  //}
//
//  // Left Thumb
//  if (buttonsCurrState_[GamepadButton::LeftThumb] == KeyState::Pressed)
//  {
//    std::cout << "Left Thumb is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::LeftThumb] == KeyState::HeldDown)
//  {
//    std::cout << "Left Thumb is held down" << std::endl;
//  }
//
//  // Right Thumb
//  if (buttonsCurrState_[GamepadButton::RightThumb] == KeyState::Pressed)
//  {
//    std::cout << "Right Thumb is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::RightThumb] == KeyState::HeldDown)
//  {
//    std::cout << "Right Thumb is held down" << std::endl;
//  }
//
//  // Directional Pad Up
//  if (buttonsCurrState_[GamepadButton::Up] == KeyState::Pressed)
//  {
//    std::cout << "DPad UP is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Up] == KeyState::HeldDown)
//  {
//    std::cout << "DPad UP is held down" << std::endl;
//  }
//
//  // Directional Pad Right
//  if (buttonsCurrState_[GamepadButton::Right] == KeyState::Pressed)
//  {
//    std::cout << "DPad RIGHT is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Right] == KeyState::HeldDown)
//  {
//    std::cout << "DPad RIGHT is held down" << std::endl;
//  }
//
//  // Directional Pad Down
//  if (buttonsCurrState_[GamepadButton::Down] == KeyState::Pressed)
//  {
//    std::cout << "DPad DOWN is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Down] == KeyState::HeldDown)
//  {
//    std::cout << "DPad DOWN is held down" << std::endl;
//  }
//
//  // Directional Pad Left
//  if (buttonsCurrState_[GamepadButton::Left] == KeyState::Pressed)
//  {
//    std::cout << "DPad LEFT is pressed" << std::endl;
//  }
//  else if (buttonsCurrState_[GamepadButton::Left] == KeyState::HeldDown)
//  {
//    std::cout << "DPad LET is held down" << std::endl;
//  }
//}

// Debug for array of button states
void Gamepad::BitPrint(const unsigned char * const array, int count)
{
  for (int i = 0; i < count; ++i)
  {
    if (array[i])
    {
      std::cout << "1 ";
    }
    else
    {
      std::cout << "0 ";
    }
  }
  std::cout << std::endl;
}

