/*******************************************************
FILE: Keybinding.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "KeyMap.h"
#include "Commands.h"

// Holds the key and the command to execute for each player
class Keybinding
{
  public:
    Keybinding();
    ~Keybinding();

    void ResetKeybinding();
    Key GetKeybinding(Action action) const;

    void SetJump(Key jump);
    void SetAttack(Key attack);
    void SetMoveLeft(Key left);
    void SetMoveRight(Key right);
};

class KeybindingKeyboard
{
  private:
    PlayerID playerID_;
    Key jump_;
    Key attack_;
    Key stack_;
    Key moveLeft_;
    Key moveRight_;
    Command * jumpCommand_;
    Command * attackCommand_;
    Command * stackCommand_;
    Command * moveCommand_;

  public:

};

class KeybindingGamepad
{
  private:
    PlayerID playerID_;
    GamepadButton jump_;
    GamepadButton attack_;
    GamepadButton stack_;
    GamepadAxis move_;
    Command * jumpCommand_;
    Command * attackCommand_;
    Command * stackCommand_;
    Command * moveCommand_;

  public:

};