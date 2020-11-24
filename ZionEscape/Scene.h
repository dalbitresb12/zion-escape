#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "SceneSpawner.h"
#include "BitmapManager.h"
#include "Enums.h"
#include "DataTypes.h"

using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Scene {
  Bitmap^ background;
  List<Direction>^ doors;
  Dictionary<Direction, Scene^>^ neighbours;
  Dictionary<Direction, SceneSpawner^>^ spawners;
  // Only used to check collisions
  Point position;

public:
  Scene(DoorLocations doorLocations, Point position) {
    this->InitDoorsList(doorLocations);
    this->neighbours = gcnew Dictionary<Direction, Scene^>;
    this->spawners = gcnew Dictionary<Direction, SceneSpawner^>;
    this->position = position;
    ImageSelector();
  }

  ~Scene() {
    if (doors != nullptr) {
      doors->Clear();
      delete doors;
    }

    if (spawners != nullptr) {
      spawners->Clear();
      delete spawners;
    }

    if (neighbours != nullptr) {
      neighbours->Clear();
      delete neighbours;
    }
  }

  void SetBackground(BackgroundImage image) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    background = bmpManager->GetImage(EnumUtilities::GetPathFromBackground(image));
  }

  //Temporal Image Selector -> Works as  a reference
  void ImageSelector() {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    bool up = GetDoorValue(Direction::Up);
    bool down = GetDoorValue(Direction::Down);
    bool left = GetDoorValue(Direction::Left);
    bool right = GetDoorValue(Direction::Right);

    if (!up && down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\D.png");

    else if (!up && down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DR.png");

    else if (up && down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\G.png");

    else if (!up && !down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\L.png");

    else if (!up && !down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\RL.png");

    else if (!up && !down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\R.png");

    else if (up && !down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\T.png");

    else if (up && down && !right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TD.png");

    else if (up && !down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TL.png");

    else if (up && !down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TR.png");

    else if (!up && down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DL.png");

    else if (!up && down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\DRL.png");

    else if (up && down && !right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TDL.png");

    else if (up && down && right && !left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TDR.png");

    else if (up && !down && right && left)
      background = bmpManager->GetImage("assets\\sprites\\colliders\\TRL.png");
  }

  void Draw(Graphics^ world) {
    Point worldPos = Point((position.X + 10) * background->Width, (position.Y + 10) * background->Height);
    world->DrawImage(background, worldPos);
  }

  void CreateSpawners(Dictionary<Point, int>^ points, Random^ rnd, Scene^ parentScene, Direction parentDirection) {
    AddNeighbour(parentDirection, parentScene);

    // Shuffle doors List
    ShuffleDoors(rnd);

    for each (Direction direction in doors) {
      if (direction == parentDirection)
        continue;

      CreateSpawner(points, direction);
    }
  }

  void CreateSpawners(Dictionary<Point, int>^ points, Random^ rnd) {
    // Shuffle doors List
    ShuffleDoors(rnd);

    for each (Direction direction in doors) {
      CreateSpawner(points, direction);
    }
  }

  void DeleteSpawner(Direction direction) {
    if (spawners->ContainsKey(direction)) {
      SceneSpawner^ element;
      if (spawners->TryGetValue(direction, element))
        delete element;
      spawners->Remove(direction);
    }
  }

  void AddNeighbour(Direction direction, Scene^ scene) {
    neighbours->Add(direction, scene);
  }

  Dictionary<Direction, Scene^>^ GetNeighbours() {
    return neighbours;
  }

  void SetDoorValue(Direction direction, bool value) {
    if (value && !doors->Contains(direction))
      doors->Add(direction);
    else if (!value && doors->Contains(direction))
      doors->Remove(direction);
    this->ImageSelector();
  }

  bool GetDoorValue(Direction direction) {
    return doors->Contains(direction);
  }

  DoorLocations GetDoorLocations() {
    return DoorLocations(GetDoorValue(Direction::Up), GetDoorValue(Direction::Down),
                         GetDoorValue(Direction::Left), GetDoorValue(Direction::Right));
  }

  Point GetPos() {
    return position;
  }

  Size GetBackgroundSize() {
    return background->Size;
  }

  Dictionary<Direction, SceneSpawner^>^ GetSpawners() {
    return spawners;
  }

private:
  void InitDoorsList(DoorLocations locations) {
    if (doors != nullptr) {
      doors->Clear();
    } else {
      doors = gcnew List<Direction>;
    }

    if (locations.Up) doors->Add(Direction::Up);
    if (locations.Down) doors->Add(Direction::Down);
    if (locations.Left) doors->Add(Direction::Left);
    if (locations.Right) doors->Add(Direction::Right);
  }

  void CreateSpawner(Dictionary<Point, int>^ points, Direction direction) {
    Point pos = EnumUtilities::GetPositionFromDirection(position, direction, 1);
    Direction inverse = EnumUtilities::GetInverseDirection(direction);
    SceneSpawner^ spawner = gcnew SceneSpawner(inverse, pos);
    AddSpawner(direction, spawner);
    if (!points->ContainsKey(pos))
      points->Add(pos, spawner->GetHashCode());
  }

  void AddSpawner(Direction direction, SceneSpawner^ spawner) {
    DeleteSpawner(direction);

    if (!spawners->ContainsKey(direction))
      spawners->Add(direction, spawner);
  }

  void ShuffleDoors(Random^ rnd) {
    // Fisher-Yates shuffle
    // See more: https://stackoverflow.com/a/1262619
    short n = doors->Count;
    while (n > 1) {
      n--;
      short k = rnd->Next(n + 1);
      Direction value = doors[k];
      doors[k] = doors[n];
      doors[n] = value;
    }
  }
};

#endif // !_SCENE_H_


