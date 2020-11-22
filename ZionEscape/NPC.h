#pragma once

#ifndef _NPC_H_
#define _NPC_H_

#include "Entity.h"
#include "Node.h"

using namespace System::Collections::Generic;

ref class NPC abstract : public Entity {
  bool pathEnded;

public:
  List<Node^>^ path;

  NPC(EntityType entityType, Point pos, unsigned short velocity, float healthPoints, float damagePoints)
    : Entity(entityType, pos, velocity, healthPoints, damagePoints) {}

  bool HasEndedPath() {
    return pathEnded;
  }

  Point GetDelta() {
    if (path == nullptr || !(path->Count > 0))
      return Point(0, 0);

    Point currentWaypoint = path[path->Count - 1]->worldPos;

    int deltaX = currentWaypoint.X - drawingArea.X;
    int deltaY = currentWaypoint.Y - drawingArea.Y;

    return Point(deltaX, deltaY);
  }

  void Move(int deltaX, int deltaY) override {
    if (path == nullptr || !(path->Count > 0)) {
      StopAnimation();
      return;
    }

    StartAnimation();

    if (deltaX != 0)
      drawingArea.X += deltaX < 0 ? -velocity : velocity;
    if (deltaY != 0)
      drawingArea.Y += deltaY < 0 ? -velocity : velocity;


    Point currentWaypoint = path[path->Count - 1]->worldPos;

    if (drawingArea.Location == currentWaypoint) {
      path->Remove(path[path->Count - 1]);
    }

    for each (Direction direction in GetDirectionFromDelta(deltaX, deltaY))
      this->SetSpriteDirection(direction);
  }
};

#endif // !_NPC_H_
