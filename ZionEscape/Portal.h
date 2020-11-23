#pragma once

#ifndef _PORTAL_H_
#define _PORTAL_H_

#include "Entity.h"
#include "BitmapManager.h"

ref class Portal : public Entity {
  short cooldown;
public:
  Portal(Point pos)
    : Entity(EntityType::Obstacle, pos, 0U, 0.f, 0.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\misc\\portal.png");
    this->SetImage(image, 9, 1);
    this->SetAnimatable(true);
    this->cooldown = 50;
  }
  ~Portal() {}

  void SetCooldown(short value) {
    this->cooldown = value;
  }

  short GetCooldown() {
    return this->cooldown;
  }

  void SetSpriteDirection(Direction direction) override {}
};

#endif // !_PORTAL_H_
