/*******************************************************
FILE: Keybinding.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Keybinding.h"

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
    case Action::MoveLeft:
    {
      return gamepad_;
    }
    case Action::MoveRight:
    {
      return moveRight_;
    }
  }
}

void Keybinding::SetKeyboardJump(Key key)
{
  jump_ = jump;
}

void Keybinding::SetKeyboardAttack(Key key)
{
  attack_ = attack;
}

void Keybinding::SetKeyboardStack(Key key)
{
}

void Keybinding::SetKeyboardMoveLeft(Key key)
{
  moveLeft_ = left;
}

void Keybinding::SetKeyboardMoveRight(Key key)
{
  moveRight_ = right;
}

void Keybinding::SetGamepadJump(GamepadButton button)
{
  jump_ = jump;
}

void Keybinding::SetGamepadAttack(GamepadButton button)
{
  attack_ = attack;
}

void Keybinding::SetGamepadStack(GamepadButton button)
{
}

void Keybinding::SetGamepadMove(GamepadAxis axis)
{
  moveLeft_ = left;
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
}

void KeybindingKeyboard::SetAttack(Key key)
{
}

void KeybindingKeyboard::SetStack(Key key)
{
}

void KeybindingKeyboard::SetMoveLeft(Key key)
{
}

void KeybindingKeyboard::SetMoveRight(Key key)
{
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