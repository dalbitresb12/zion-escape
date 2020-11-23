#pragma once

#ifndef _ASSASSIN_H_
#define _ASSASSIN_H_

#include "NPC.h"
#include "Entity.h"
#include "BitmapManager.h"

ref class Assassin : public NPC {
public:
  Assassin(Point pos)
    : NPC(EntityType::Assassin, pos, 3U, 3.f, 1.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\asesinos\\asesino.png");
    this->SetImage(image, 3, 4);
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
};

#endif // !_ASSASSIN_H_

