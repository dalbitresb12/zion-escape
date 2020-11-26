#pragma once

#ifndef _BULLET_H_
#define _BULLET_H_

#include "Entity.h"

ref class Bullet: public Entity {
  float floatX, floatY, speed;
  double angle;

public:
  Bullet(Size size, float posX, float posY, float posTargetX, float posTargetY, float speed)
    : Entity(EntityType::Obstacle,Point(posX, posY), speed, 0.f, 0.f) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\misc\\bullet.png");
    this->SetImage(image, 1, 1);
    //Position of the bullet (float)
    this->floatX = this->drawingArea.X;
    this->floatY = this->drawingArea.Y;
    //Calculate the angle between the player and the mouse
    this->angle = Math::Atan2(this->floatY - posTargetY, posTargetX - this->floatX);
    //Set the speed
    this->speed = speed;
  }

  ~Bullet(){}

  void Move() override {
    //Set a new position for the bullet
    this->floatX += Math::Cos(this->angle) * this->speed;
    this->floatY -= Math::Sin(this->angle) * this->speed;
    //Round the value of the float
    this->SetPosition(Point(Math::Round(floatX), Math::Round(floatY)));
  }

  bool OutScreen(Rectangle area) {
    //Detect if the bullet is out of the area
    if (this->drawingArea.X > area.Width || this->drawingArea.X + this->drawingArea.Width < 0) return true;
    else if (this->drawingArea.Y > area.Height || this->drawingArea.Y + this->drawingArea.Height < 0) return true;
    return false;
  }

  void SetSpriteDirection(Direction direction) override{}

};

#endif // !_BULLET_H_
