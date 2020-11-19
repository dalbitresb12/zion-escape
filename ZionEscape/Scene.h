#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "SceneSpawner.h"
#include "BitmapManager.h"

using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Scene {
  Bitmap^ background;
  bool up, down, right, left;
  List<SceneSpawner^>^ spawners;
  Rectangle drawingArea;

public:
  Scene(bool up, bool down, bool left, bool right, Point pos) {
    this->spawners = gcnew List<SceneSpawner^>;
    this->up = up;
    this->down = down;
    this->right = right;
    this->left = left;
    this->ImageSelector();
    this->drawingArea = Rectangle(pos, this->background->Size);
  }

  ~Scene() {
    for each (SceneSpawner ^ spawner in this->spawners)
      delete spawner;
    this->spawners->Clear();
    delete this->spawners;
  }

  //Temporal Image Selector -> Works as  a reference
  void ImageSelector() {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    this->background = bmpManager->GetImage("assets\\sprites\\scenes\\scene_1.png");
  }

  void Draw(Graphics^ g) {
    g->DrawImage(this->background, this->drawingArea);
  }

  void DrawDoors(Graphics^ g) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    if (this->up)
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\doors\\up_door.png"),this->GetPos().X+422, this->GetPos().Y+27);
    if (this->down)
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\doors\\down_door.png"), this->GetPos().X + 422, this->GetPos().Y + 520);
    if (this->right)
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\doors\\right_door.png"), this->GetPos().X + 831, this->GetPos().Y + 266);
    if (this->left)
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\doors\\left_door.png"), this->GetPos().X + 27, this->GetPos().Y + 266);
  }

  void CreateSpawner(Point pos) {
    Point location;
    Size size = Size(this->background->Width, this->background->Height);
    if (this->up) {
      location = Point(pos.X, pos.Y - size.Height);
      this->spawners->Add(gcnew SceneSpawner(Direction::Down, Rectangle(location, size)));
    }
    if (this->down) {
      location = Point(pos.X, pos.Y + size.Height);
      this->spawners->Add(gcnew SceneSpawner(Direction::Up, Rectangle(location, size)));
    }
    if (this->right) {
      location = Point(pos.X + size.Width, pos.Y);
      this->spawners->Add(gcnew SceneSpawner(Direction::Left, Rectangle(location, size)));
    }
    if (this->left) {
      location = Point(pos.X - size.Width, pos.Y);
      this->spawners->Add(gcnew SceneSpawner(Direction::Right, Rectangle(location, size)));
    }
  }

  void DeleteSpawner(short n) {
    this->spawners->Remove(this->spawners[n]);
  }

  void SetPosition(Point pos) {
    this->drawingArea.Location = pos;
  }

  void SetUp(bool value) {
    this->up = value;
    this->ImageSelector();
  }

  void SetDown(bool value) {
    this->down = value;
    this->ImageSelector();
  }

  void SetRight(bool value) {
    this->right = value;
    this->ImageSelector();
  }

  void SetLeft(bool value) {
    this->left = value;
    this->ImageSelector();
  }

  bool GetUp() {
    return this->up;
  }

  bool GetDown() {
    return this->down;
  }

  bool GetRight() {
    return this->right;
  }

  bool GetLeft() {
    return this->left;
  }

  Rectangle GetDrawingArea() {
    return this->drawingArea;
  }

  Point GetPos() {
    return this->drawingArea.Location;
  }

  List<SceneSpawner^>^ GetSpawners() {
    return this->spawners;
  }
};

#endif // !_SCENE_H_


