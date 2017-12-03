/*******************************************************
FILE: Keybinding.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Keybinding.h"
#include <iostream>

// Initialize keybinding
Keybinding::Keybinding(PlayerID playerID)
           : playerID_(playerID)
           , keyboard_(new KeybindingKeyboard(playerID))
           , gamepad_(new KeybindingGamepad)
{
}

// Free memory
Keybinding::~Keybinding()
{
  delete keyboard_;
  delete gamepad_;
}

// Reset everything to default
void Keybinding::ResetAllKeybinding()
{
  keyboard_->ResetDefault();
  gamepad_->ResetDefault();
}

void Keybinding::ResetKeyboardKeybinding()
{
  keyboard_->ResetDefault();
}

void Keybinding::ResetGamepadKeybinding()
{
  gamepad_->ResetDefault();
}

Key Keybinding::GetKeyboardKeybinding(Action action) const
{
  switch (action)
  {
    case Action::Attack :
    {
      return keyboard_->GetAttack();
    }
    case Action::Jump :
    {
      return keyboard_->GetJump();
    }
    case Action::MoveLeft :
    {
      return keyboard_->GetMoveLeft();
    }
    case Action::MoveRight :
    {
      return keyboard_->GetMoveRight();
    }
  }
}

GamepadButton Keybinding::GetGamepadKeybinding(Action action) const
{
  switch (action)
  {
    case Action::Attack:
    {
      return gamepad_->GetAttack();
    }
    case Action::Jump:
    {
      return gamepad_->GetJump();
    }
    case Action::Toss:
    {
      return gamepad_->GetToss();
    }
  }
}

GamepadAxis Keybinding::GetGamepadKeybindingAxis(Action action) const
{
  if (action == Action::MoveLeft)
  {
    return gamepad_->GetMoveAxisX();
  }
  else
  {
    return gamepad_->GetMoveAxisY();
  }
}

void Keybinding::SetKeyboardJump(Key key)
{
  keyboard_->SetJump(key);
}

void Keybinding::SetKeyboardAttack(Key key)
{
  keyboard_->SetAttack(key);
}

void Keybinding::SetKeyboardStack(Key key)
{
  // There's no stack button why is this here
}

void Keybinding::SetKeyboardMoveLeft(Key key)
{
  keyboard_->SetMoveLeft(key);
}

void Keybinding::SetKeyboardMoveRight(Key key)
{
  keyboard_->SetMoveRight(key);
}

void Keybinding::SetGamepadJump(GamepadButton button)
{
  gamepad_->SetJump(button);
}

void Keybinding::SetGamepadAttack(GamepadButton button)
{
  gamepad_->SetAttack(button);
}

void Keybinding::SetGamepadStack(GamepadButton button)
{
  // No staaaaack
}

void Keybinding::SetGamepadMoveButton(GamepadButton button, Action action)
{
  if (action != Action::MoveLeft || action != Action::MoveRight)
  {
    printf("Cannot set move button: Invalid action");
    return;
  }

  gamepad_->SetMoveButton(button, action);
}

void Keybinding::SetGamepadMoveAxis(GamepadAxis axis)
{
  gamepad_->SetMove(axis);
}

//=============================================
//  Keyboard Keybinding
//=============================================
KeybindingKeyboard::KeybindingKeyboard(PlayerID playerID)
                  : playerID_(playerID)
{
  switch (playerID)
  {
    case Player1:
    {
      jump_      = Key::W;
      attack_    = Key::T;
      stack_     = Key::Y;
      moveLeft_  = Key::A;
      moveRight_ = Key::D;
    }
    case Player2:
    {
      jump_      = Key::Up;
      attack_    = Key::Numpad_5;
      stack_     = Key::Numpad_6;
      moveLeft_  = Key::Left;
      moveRight_ = Key::Right;
    }
    // TODO: Find a better default configuration for players 3 and 4
    case Player3:
    {
      jump_      = Key::Up;
      attack_    = Key::Numpad_5;
      stack_     = Key::Numpad_6;
      moveLeft_  = Key::Left;
      moveRight_ = Key::Right;
    }
    case Player4:
    {
      jump_      = Key::Up;
      attack_    = Key::Numpad_5;
      stack_     = Key::Numpad_6;
      moveLeft_  = Key::Left;
      moveRight_ = Key::Right;
    }
  }

}

KeybindingKeyboard::~KeybindingKeyboard()
{
}

void KeybindingKeyboard::SetJump(Key key)
{
  jump_ = key;
}

void KeybindingKeyboard::SetAttack(Key key)
{
  attack_ = key;
}

void KeybindingKeyboard::SetStack(Key key)
{
  stack_ = key;
}

void KeybindingKeyboard::SetMoveLeft(Key key)
{
  moveLeft_ = key;
}

void KeybindingKeyboard::SetMoveRight(Key key)
{
  moveRight_ = key;
}

Key KeybindingKeyboard::GetJump()
{
  return jump_;
}


Key KeybindingKeyboard::GetAttack()
{
  return attack_;
}

Key KeybindingKeyboard::GetStack()
{
  return stack_;
}

Key KeybindingKeyboard::GetMoveLeft()
{
  return moveLeft_;
}

Key KeybindingKeyboard::GetMoveRight()
{
  return moveRight_;
}

//=============================================
//  Gamepad Keybinding
//=============================================
KeybindingGamepad::KeybindingGamepad()
{
}

KeybindingGamepad::~KeybindingGamepad()
{
}