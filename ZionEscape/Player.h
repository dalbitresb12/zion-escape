#pragma once

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "BitmapManager.h"
#include "Entity.h"

ref class Player : public Entity {
public:
  Player(Point pos)
    : Entity(EntityType::Player, pos, 3, 10.f, 2.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\principal\\principal_m.png");
    this->SetImage(image, 4, 4);
  }

  Player(Bitmap^ image, short nCols, short nRows, Point pos)
    : Entity(EntityType::Player, pos, 3, 10.f, 2.f) {
    this->SetImage(image, nCols, nRows);
  }

  ~Player() {}

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

#endif // !_PLAYER_H_

