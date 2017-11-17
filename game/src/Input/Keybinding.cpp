/*******************************************************
FILE: Keybinding.cpp

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#include "Keybinding.h"

// Default keybinding for keyboard
Keybinding::Keybinding() : jump_(Key::W)
                         , attack_(Key::RightShift)
                         , stack_(Key::RightControl)
                         , moveLeft_(Key::A)
                         , moveRight_(Key::D)
{
}

Keybinding::~Keybinding()
{
}

// Reset to default
void Keybinding::ResetKeybinding()
{
  // TODO: Change this to accommodate gamepad
  jump_ = Key::Space;
  attack_ = Key::RightShift;
  moveLeft_ = Key::A;
  moveRight_ = Key::D;
}

Key Keybinding::GetKeybinding(Action action) const
{
  switch (action)
  {
    case Action::Attack :
    {
      return attack_;
    }
    case Action::Jump :
    {
      return jump_;
    }
    case Action::MoveLeft :
    {
      return moveLeft_;
    }
    case Action::MoveRight :
    {
      return moveRight_;
    }
  }
}

void Keybinding::SetJump(Key jump)
{
  jump_ = jump;
}

void Keybinding::SetAttack(Key attack)
{
  attack_ = attack;
}

void Keybinding::SetMoveLeft(Key left)
{
  moveLeft_ = left;
}

void Keybinding::SetMoveRight(Key right)
{
  moveRight_ = right;
}
