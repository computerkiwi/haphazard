/*******************************************************
FILE: Keybinding.h

PRIMARY AUTHOR: Lya Vera

PROJECT:

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "KeyMap.h"

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

  private:
    Key jump_;
    Key attack_;
    Key moveLeft_;
    Key moveRight_;
};