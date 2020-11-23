#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "Map.h"
#include "Player.h"

ref class Game {
  Map^ map;

public:
  Game(Size screenArea) {
    this->map = gcnew Map(screenArea);
  }
  ~Game() {
    delete map;
  }

  void StartGeneration(Graphics^ g, Player^ player) {
    this->map->StartGeneration(g, player);
    this->map->MoveMap(player);
  }

  bool IsGenerated() {
    return this->map->IsGenerated();
  }

  int GetMapSeed() {
    return this->map->GetSeed();
  }
};

#endif // !_GAME_H_


