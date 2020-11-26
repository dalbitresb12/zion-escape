#pragma once

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "BitmapManager.h"
#include "Entity.h"
#include "Bullet.h"

using namespace System::Collections::Generic;

using namespace System;

namespace Defaults {
  namespace Player {
    const unsigned short Velocity = 3;
    const float InitialHealth = 10.f;
    const float DamagePoints = 2.f;
  }
}

ref class Player : public Entity {
  array<Keys>^ movementKeys;
  List<Keys>^ keysPressed;
  List<Bullet^>^ bullets;
public:
  Player(Point pos)
    : Entity(EntityType::Player, pos, Defaults::Player::Velocity, Defaults::Player::InitialHealth, Defaults::Player::DamagePoints) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ image = bmpManager->GetImage("assets\\sprites\\principal\\principal_m.png");
    this->bullets = gcnew List<Bullet^>;
    this->SetImage(image, 4, 4);
    this->movementKeys = gcnew array<Keys> { Keys::A, Keys::D, Keys::S, Keys::W };
    this->keysPressed = gcnew List<Keys>;
  }

  Player(Bitmap^ image, short nCols, short nRows, Point pos)
    : Entity(EntityType::Player, pos, Defaults::Player::Velocity, Defaults::Player::InitialHealth, Defaults::Player::DamagePoints) {
    this->SetImage(image, nCols, nRows);
  }

  ~Player() {
    for each (Bullet ^ bullet in this->bullets)
      delete bullet;
    this->bullets->Clear();
    delete this->bullets;
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
    }
  }

  void KeyDown(KeyEventArgs^ e) {
    if (!IsValidKey(e->KeyCode))
      return;

    if (!keysPressed->Contains(e->KeyCode)) {
      keysPressed->Add(e->KeyCode);
      StartAnimation();
    }
  }

  //Create a Bullet
  void Shoot(float posX, float posY) {
    this->bullets->Add(gcnew Bullet(Size(20,20), this->drawingArea.X+10, this->drawingArea.Y+20, posX, posY, 10));
  }

  void ActionBullets(Graphics^ g, Rectangle area, List<NPC^>^ npcs) {
    //Check if there are bullets
    if (this->bullets->Count > 0)
      //Check all the Bullets in the List
      for (unsigned currentBullet = this->bullets->Count; currentBullet > 0; currentBullet--) {

        bool isDeleted = false;
        //Move the bullet
        bullets[currentBullet-1]->Move();
        //Draw the bullet
        bullets[currentBullet-1]->Draw(g);

        //Check if the bullet collides with any Corrupt or Assasin
        for each (NPC ^ npc in npcs)
          if (npc->GetEntityType() == EntityType::Corrupt || npc->GetEntityType() == EntityType::Assassin)
            if (bullets[currentBullet - 1]->HasCollision(npc)) {
              npc->SetHealth(npc->GetHealth() - this->GetDamagePoints());
              isDeleted = true;
              break;
            }
       
        //Detects if the Bullet is out of the screen
        if (bullets[currentBullet-1]->OutScreen(area))
          isDeleted = true;

        if (isDeleted) {
          //Delete ptr
          this->bullets[currentBullet - 1] = nullptr;
          delete this->bullets[currentBullet - 1];
          //Delete form the list
          bullets->Remove(bullets[currentBullet - 1]);
        }
      }
  }

  void ActionBullets(Graphics^ g, Rectangle area) {
    //Check if there are bullets
    if (this->bullets->Count > 0)
      //Check all the Bullets in the List
      for (unsigned currentBullet = this->bullets->Count; currentBullet > 0; currentBullet--) {

        bool isDeleted = false;
        //Move the bullet
        bullets[currentBullet - 1]->Move();
        //Draw the bullet
        bullets[currentBullet - 1]->Draw(g);

        //Detects if the Bullet is out of the screen
        if (bullets[currentBullet - 1]->OutScreen(area))
          isDeleted = true;

        if (isDeleted) {
          //Delete ptr
          this->bullets[currentBullet - 1] = nullptr;
          delete this->bullets[currentBullet - 1];
          //Delete form the list
          bullets->Remove(bullets[currentBullet - 1]);
        }
      }
  }


  void KeyUp(KeyEventArgs^ e) {
    if (keysPressed->Contains(e->KeyCode))
      keysPressed->Remove(e->KeyCode);

    if (keysPressed->Count == 0)
      StopAnimation();
  }

  bool MoveUsingKeysList(GraphicsPath^ walkableLayer) {
    if (keysPressed->Count == 0) {
      return false;
    }

    for each (Keys key in keysPressed) {
      if (!IsValidKey(key)) break;
      Move(key, walkableLayer);
    }

    return true;
  }

private:
  bool IsValidKey(Keys key) {
    // Array::BinarySeach needs a sorted array to work correctly
    // See more in the link below:
    // https://docs.microsoft.com/en-us/dotnet/api/system.array.binarysearch?view=netframework-4.8
    return !(Array::BinarySearch(movementKeys, key) < 0);
  }
};

#endif // !_PLAYER_H_

