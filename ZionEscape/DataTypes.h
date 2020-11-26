#pragma once

#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include "BitmapManager.h"

class DoorLocations {
public:
  bool Up, Down, Left, Right;

  DoorLocations() {
    this->SetAll(false, false, false, false);
  }

  DoorLocations(bool value) {
    this->SetAll(value);
  }

  DoorLocations(bool up, bool down, bool left, bool right) {
    this->SetAll(up, down, left, right);
  }

  ~DoorLocations() {}

  short Count() {
    return (Up ? 1 : 0) + (Down ? 1 : 0) + (Left ? 1 : 0) + (Right ? 1 : 0);
  }

  bool IsAllTrue() {
    return Up && Down && Left && Right;
  }

  void SetAll(bool value) {
    SetAll(value, value, value, value);
  }

  void SetAll(bool x, bool y) {
    SetAll(y, y, x, x);
  }

  void SetAll(bool up, bool down, bool left, bool right) {
    this->Up = up;
    this->Down = down;
    this->Left = left;
    this->Right = right;
  }

  // TO DO: Remove when minimap is no longer needed
  static Bitmap^ GetImage(DoorLocations doors) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ background;
    bool up = doors.Up;
    bool down = doors.Down;
    bool left = doors.Left;
    bool right = doors.Right;

    if (!up && down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\D.png");

    else if (!up && down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DR.png");

    else if (up && down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\G.png");

    else if (!up && !down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\L.png");

    else if (!up && !down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\RL.png");

    else if (!up && !down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\R.png");

    else if (up && !down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\T.png");

    else if (up && down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TD.png");

    else if (up && !down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TL.png");

    else if (up && !down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TR.png");

    else if (!up && down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DL.png");

    else if (!up && down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DRL.png");

    else if (up && down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TDL.png");

    else if (up && down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TDR.png");

    else if (up && !down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TRL.png");

    return background;
  }
};

template<typename T>
ref class MinMax {
public:
  T Min;
  T Max;

  MinMax(T Min, T Max) {
    this->Min = Min;
    this->Max = Min;
  }
  ~MinMax() {}
};

#endif // !_DATATYPES_H_
