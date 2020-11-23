#pragma once

#ifndef _ALLY_H_
#define _ALLY_H_

#include "NPC.h"
#include "Entity.h"
#include "BitmapManager.h"

using namespace System;
using namespace System::Drawing;

ref class Ally : public NPC {
public:
  Ally(Point pos)
    : NPC(EntityType::Ally, pos, 2U, 1.f, 0.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\aliados\\aliado-bueno.png");
    this->SetImage(image, 4, 4);
  }
  ~Ally() {}

  bool ShouldDie() {
    Random r;
    int value = r.Next(1, 101); // (inicio, final + 1)
    return value > 57; //  (100 - num / 100) = % prob
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

#endif // !_ALLY_H_
