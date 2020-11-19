#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "SceneSpawner.h"

using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Scene {
  bool up, down, right, left;
  int background;
  List<SceneSpawner^>^ spawners;
  Rectangle drawingArea;

public:
  Scene(bool up, bool down, bool left, bool right, int bg, Point pos, Size area) {
    this->spawners = gcnew List<SceneSpawner^>;
    this->up = up;
    this->down = down;
    this->right = right;
    this->left = left;
    this->background = bg;
    this->drawingArea = Rectangle(pos, area);
  }

  ~Scene() {
    for each (SceneSpawner ^ spawner in this->spawners)
      delete spawner;
    this->spawners->Clear();
    delete this->spawners;
  }

  void CreateSpawner(Point pos) {
    Point location;
    Size size = Size(this->drawingArea.Width, this->drawingArea.Height);
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
  }

  void SetDown(bool value) {
    this->down = value;
  }

  void SetRight(bool value) {
    this->right = value;
  }

  void SetLeft(bool value) {
    this->left = value;
  }

  int GetBackground() {
    return this->background;
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


