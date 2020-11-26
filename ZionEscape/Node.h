#pragma once

#ifndef _NODE_H_
#define _NODE_H_

using namespace System;
using namespace System::Drawing;

ref class Node {
public:
  Point worldPos;
  Point gridPos;
  bool walkable;
  int gCost;
  int hCost;
  Node^ parent;

  Node(Point worldPos, Point gridPos, bool walkable) {
    this->worldPos = worldPos;
    this->gridPos = gridPos;
    this->walkable = walkable;

    this->gCost = 0;
    this->hCost = 0;
    this->parent = nullptr;
  }
  ~Node() {
    if (parent != nullptr)
      delete parent;
  }
  // TO DO: Change to property get
  // https://docs.microsoft.com/en-us/cpp/dotnet/how-to-use-properties-in-cpp-cli?view=msvc-160
  int GetFCost() {
    return this->gCost + this->hCost;
  }
};

#endif // !_NODE_H_
