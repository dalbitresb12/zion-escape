#pragma once
#include "Map.h"

ref class Game {
  Map^ map;

public:
  Game(Graphics^ g) {
    this->map = gcnew Map(g);
  }

  void Generation(Graphics^ g) {
    this->map->MapGeneration(g);
  }

  bool IsGenerated() {
    return this->map->GetGenerated();
  }
};
