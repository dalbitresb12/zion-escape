#pragma once

#include "Entity.h"

ref class Portal: public Entity {
public:
  Portal(Point pos)
    : Entity(entityType, pos, velocity, healthPoints, damagePoints) {}

  ~Portal(){}


};
