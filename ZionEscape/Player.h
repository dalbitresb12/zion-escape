#pragma once

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "BitmapManager.h"
#include "Entity.h"

ref class Player : public Entity {
public:
  Player(Point pos)
    : Entity(EntityType::Player, pos, 3U, 10.f, 2.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\principal\\principal_m.png");
    this->SetImage(image, 4, 4);
  }

  Player(Bitmap^ image, short nCols, short nRows, Point pos)
    : Entity(EntityType::Player, pos, 3U, 10.f, 2.f) {
    this->SetImage(image, nCols, nRows);
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

  bool Collides(Rectangle other) {
    return this->drawingArea.IntersectsWith(other);
  }

  void DrawHearts(Graphics^ g) {
    Point heartPos = Point(20, 15);
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    //Draw all the healthPoints of the player
    for (float i = 0.f; i < this->healthPoints; i++) {
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\misc\\heart.png"), heartPos);
      //Add the X position of where the next heart will be drawn
      heartPos.X += 46;
    }
  }
};

#endif // !_PLAYER_H_

