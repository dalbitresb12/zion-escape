#pragma once

#ifndef _ASSASSIN_H_
#define _ASSASSIN_H_

#include "NPC.h"
#include "Entity.h"
#include "BitmapManager.h"

ref class Assassin : public NPC {
  short cooldown;
public:
  Assassin(Point pos)
    : NPC(EntityType::Assassin, pos, 5U, 3.f, 1.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\asesinos\\asesino.png");
    this->SetImage(image, 3, 4);
    this->cooldown = 0;
  }
  ~Assassin() {}

  void SetSpriteDirection(Direction direction) override {
    switch (direction) {
      case Direction::Up:
        SetRow(3);
        break;
      case Direction::Down:
        SetRow(0);
        break;
      case Direction::Left:
        SetRow(1);
        break;
      case Direction::Right:
        SetRow(2);
        break;
      default:
        SetRow(0);
        break;
    }
  }

  void StopAnimation() override {
      SetAnimatable(false);
      SetCol(1);
  }

  void SetCooldown(short value) {
    this->cooldown = value;
  }

  short GetCooldown() {
    return this->cooldown;
  }
};

#endif // !_ASSASSIN_H_

