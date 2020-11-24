#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "Map.h"

using namespace System::Threading;
using namespace System::Threading::Tasks;

ref class Game {
  Map^ map;

public:
  Game() {
    this->map = gcnew Map();
  }
  ~Game() {
    delete map;
  }

  void MapGeneration() {
    if (!map->IsGenerating() && !map->IsGenerated()) {
      Action^ action = gcnew Action(map, &Map::StartGeneration);
      Task^ generator = Task::Factory->StartNew(action, TaskCreationOptions::LongRunning);
    }
  }

  void DrawMapGizmos(Graphics^ world) {
    if (this->map->IsGenerated())
      this->map->Draw(world);
  }

  bool IsGenerated() {
    return this->map->IsGenerated();
  }

  int GetMapSeed() {
    return this->map->GetSeed();
  }
};

#endif // !_GAME_H_


