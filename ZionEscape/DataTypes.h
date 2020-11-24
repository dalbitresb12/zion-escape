#pragma once

#ifndef _DATATYPES_H_
#define _DATATYPES_H_

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
