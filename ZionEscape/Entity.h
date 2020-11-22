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
  int velocity;

public:
  Entity(EntityType entityType, Point pos, int velocity, float healthPoints, float damagePoints)
    : Sprite(entityType != EntityType::Obstacle) {
    this->entityType = entityType;
    this->drawingArea.Location = pos;
    this->velocity = velocity;
    this->healthPoints = healthPoints;
    this->damagePoints = damagePoints;
    this->movable = entityType != EntityType::Obstacle;
    StopAnimation();
  }

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

  bool Collision(Entity^ object) {
    // Collision needs the drawingArea of another Entity to return if the object collides or not
    return this->drawingArea.IntersectsWith(object->drawingArea);
  }

  virtual void Move(Direction direction) {
    if (direction == Direction::Up || direction == Direction::Down)
      this->drawingArea.Y += direction == Direction::Up ? -velocity : velocity;
    if (direction == Direction::Left || direction == Direction::Right)
      this->drawingArea.X += direction == Direction::Left ? -velocity : velocity;

    this->SetSpriteDirection(direction);
  }

  virtual void Move(int deltaX, int deltaY) {
    for each (Direction direction in GetDirectionFromDelta(deltaX, deltaY))
      this->Move(direction);
  }

  virtual void Move(Keys key) {
    this->Move(GetDirectionFromKey(key));
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

  static List<int>^ GetDirectionFromDelta(int deltaX, int deltaY) {
    List<int>^ directions = gcnew List<int>;
    if (deltaY != 0)
      deltaY < 0 ? directions->Add((int)Direction::Up) : directions->Add((int)Direction::Down);
    if (deltaX != 0)
      deltaX < 0 ? directions->Add((int)Direction::Left) : directions->Add((int)Direction::Right);
    return directions;
  }
};

#endif // !_ENTITY_H_


