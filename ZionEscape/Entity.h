#pragma once

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Enums.h"
#include "Sprite.h"

using namespace System;
using namespace System::Windows::Forms;

// Entity publicly inheriting Sprite
ref class Entity abstract : public Sprite {
protected:
  EntityType entityType;
  bool movable;
  float healthPoints;
  float damagePoints;
  unsigned short velocity;

public:
  Entity(EntityType entityType, Point pos, unsigned short velocity, float healthPoints, float damagePoints)
    : Sprite(entityType != EntityType::Obstacle) {
    this->entityType = entityType;
    this->drawingArea.Location = pos;
    this->velocity = velocity;
    this->healthPoints = healthPoints;
    this->damagePoints = damagePoints;
    this->movable = entityType != EntityType::Obstacle;
    StopAnimation();
  }
  ~Entity() {}

  EntityType GetEntityType() {
    return entityType;
  }

  bool IsMovable() {
    return this->movable;
  }

  float GetHealth() {
    return this->healthPoints;
  }

  void SetHealth(float healthPoints) {
    this->healthPoints = healthPoints;
  }

  float GetDamagePoints() {
    return this->damagePoints;
  }

  bool HasCollision(Entity^ object) {
    return HasCollision(object->drawingArea);
  }

  bool HasCollision(Rectangle rect) {
    return this->drawingArea.IntersectsWith(rect);
  }

  virtual void Move(Direction direction, GraphicsPath^ walkableLayer) {
    Rectangle rect = Rectangle(drawingArea);

    if (direction == Direction::Up || direction == Direction::Down)
      rect.Y += direction == Direction::Up ? -velocity : velocity;
    if (direction == Direction::Left || direction == Direction::Right)
      rect.X += direction == Direction::Left ? -velocity : velocity;

    if (walkableLayer != nullptr) {
      if (walkableLayer->IsVisible(rect.Location) && walkableLayer->IsVisible(Point(rect.Right, rect.Bottom)) &&
          walkableLayer->IsVisible(Point(rect.Right, rect.Top)) && walkableLayer->IsVisible(Point(rect.Left, rect.Bottom))) {
        drawingArea = rect;
      }
    } else {
      drawingArea = rect;
    }

    SetSpriteDirection(direction);
  }

  virtual void Move(Direction direction) {
    Move(direction, nullptr);
  }

  virtual void Move(int deltaX, int deltaY, Grid^ grid) {
    for each (Direction direction in GetDirectionFromDelta(deltaX, deltaY))
      Move(direction);
  }

  virtual void Move(Keys key) {
    Move(GetDirectionFromKey(key));
  }

  virtual void Move(Keys key, GraphicsPath^ walkableLayer) {
    Move(GetDirectionFromKey(key), walkableLayer);
  }

  static Direction GetDirectionFromKey(Keys key) {
    switch (key) {
      case Keys::W:
        return Direction::Up;
      case Keys::S:
        return Direction::Down;
      case Keys::A:
        return Direction::Left;
      case Keys::D:
        return Direction::Right;
      default:
        return Direction::Up;
    }
  }

  static List<Direction>^ GetDirectionFromDelta(int deltaX, int deltaY) {
    List<Direction>^ directions = gcnew List<Direction>;
    if (deltaY != 0)
      deltaY < 0 ? directions->Add(Direction::Up) : directions->Add(Direction::Down);
    if (deltaX != 0)
      deltaX < 0 ? directions->Add(Direction::Left) : directions->Add(Direction::Right);
    return directions;
  }
};

#endif // !_ENTITY_H_


