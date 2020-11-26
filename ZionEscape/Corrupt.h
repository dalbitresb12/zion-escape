#pragma once

#ifndef _CORRUPT_H_
#define _CORRUPT_H_

#include "NPC.h"
#include "Ally.h"
#include "Entity.h"
#include "BitmapManager.h"

ref class Corrupt : public NPC {
public:
  Ally^ tracking;

  Corrupt(Point pos)
    : NPC(EntityType::Corrupt, pos, 2U, 5.f, 1.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\corruptos\\corrupto.png");
    this->SetImage(image, 4, 4);
  }
  ~Corrupt() {}

  void ConvertToAlly() {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\aliados\\aliado-corrupto.png");
    this->SetImage(image, 4, 4);
  }

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
};

#endif // !_CORRUPT_H_
