#pragma once

#ifndef _PATHFINDER_H_
#define _PATHFINDER_H_

#include "Grid.h"
#include "Node.h"
#include "NPC.h"

using namespace System;
using namespace System::Collections::Generic;

ref class Pathfinder {
private:
  Pathfinder() {}
  ~Pathfinder() {}

public:
  static void FindPath(Grid^ grid, Point startPos, Point targetPos, NPC^ npc) {
    Node^ startNode = grid->GetNodeFromWorldPoint(startPos);
    Node^ targetNode = grid->GetNodeFromWorldPoint(targetPos);

    List<Node^>^ openNodes = gcnew List<Node^>;
    HashSet<Node^>^ closedNodes = gcnew HashSet<Node^>;

    openNodes->Add(startNode);

    while (openNodes->Count > 0) {
      Node^ node = openNodes[0];
      for (int i = 1; i < openNodes->Count; ++i) {
        if (openNodes[i]->GetFCost() < node->GetFCost() || openNodes[i]->GetFCost() == node->GetFCost()) {
          if (openNodes[i]->hCost < node->hCost)
            node = openNodes[i];
        }
      }

      openNodes->Remove(node);
      closedNodes->Add(node);

      if (node == targetNode) {
        npc->path = RetracePath(startNode, targetNode);
        return;
      }

      for each (Node ^ neighbour in grid->GetNeighbours(node)) {
        if (!neighbour->walkable || closedNodes->Contains(neighbour))
          continue;

        int newCostToNeighbour = node->gCost + GetDistance(node, neighbour);
        if (newCostToNeighbour < neighbour->gCost || !openNodes->Contains(neighbour)) {
          neighbour->gCost = newCostToNeighbour;
          neighbour->hCost = GetDistance(neighbour, targetNode);
          neighbour->parent = node;

          if (!openNodes->Contains(neighbour))
            openNodes->Add(neighbour);
        }
      }
    }
  }

  static List<Node^>^ RetracePath(Node^ startNode, Node^ targetNode) {
    List<Node^>^ path = gcnew List<Node^>;
    Node^ currentNode = targetNode;

    while (currentNode != startNode) {
      path->Add(currentNode);
      currentNode = currentNode->parent;
    }

    return path;
  }

  static int GetDistance(Node^ a, Node^ b) {
    return GetDistance(a->gridPos, b->gridPos);
  }

  static int GetDistance(Point a, Point b) {
    int distanceX = Math::Abs(a.X - b.X);
    int distanceY = Math::Abs(a.Y - b.Y);

    if (distanceX > distanceY)
      return 14 * distanceY + 10 * (distanceX - distanceY);
    return 14 * distanceX + 10 * (distanceY - distanceX);
  }
};


#endif // !_PATHFINDER_H_
