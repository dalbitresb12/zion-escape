#pragma once

#ifndef _SCENESPAWNER_H_
#define _SCENESPAWNER_H_

#include "Enums.h"

using namespace System::Drawing;

ref class SceneSpawner {
  // The new scene must have a door looking in the direction
  Direction parentDirection;
  // Only used to check collisions
  Point position;
 
public:
  SceneSpawner(Direction parentDirection, Point position) {
    this->parentDirection = parentDirection;
    this->position = position;
  }

  ~SceneSpawner() {}

  Direction GetParentDirection() {
    return parentDirection;
  }

  Point GetPos() {
    return position;
  }
};

#endif // _SCENESPAWNER_H_
