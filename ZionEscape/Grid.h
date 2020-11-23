#pragma once

#ifndef _GRID_H_
#define _GRID_H_

#include "MathUtils.h"
#include "Node.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Collections::Generic;
using namespace MathUtils;

ref class Grid {
  GraphicsPath^ unwalkableLayer;
  Point gridWorldSize;
  array<Node^, 2>^ grid;

  PointF nodeRadius;
  PointF nodeDiameter;
  Point gridSize;

public:
  Grid(GraphicsPath^ unwalkableLayer, Point gridWorldSize, PointF nodeRadius) {
    this->unwalkableLayer = unwalkableLayer;
    this->gridWorldSize = gridWorldSize;
    this->nodeRadius = nodeRadius;

    nodeDiameter = PointF(nodeRadius.X * 2, nodeRadius.Y * 2);
    gridSize = Point::Round(PointF(gridWorldSize.X / nodeDiameter.X, gridWorldSize.Y / nodeDiameter.Y));
    CreateGrid();
  }
  ~Grid() {
    delete unwalkableLayer;
  }

private:
  void CreateGrid() {
    grid = gcnew array<Node^, 2>(gridSize.X, gridSize.Y);

    for (int x = 0; x < gridSize.X; ++x) {
      for (int y = 0; y < gridSize.Y; ++y) {
        Point gridPos = Point(x, y);
        Point worldPos = Point::Round(PointF(x * nodeDiameter.X + nodeRadius.X, y * nodeDiameter.Y + nodeRadius.Y));
        bool walkable = !unwalkableLayer->IsVisible(worldPos);
        grid[x, y] = gcnew Node(worldPos, gridPos, walkable);
      }
    }
  }

public:
  List<Node^>^ GetNeighbours(Node^ node) {
    List<Node^>^ neighbours = gcnew List<Node^>;

    for (int x = -1; x <= 1; ++x) {
      for (int y = -1; y <= 1; ++y) {
        if (x == 0 && y == 0)
          continue;

        int checkX = node->gridPos.X + x;
        int checkY = node->gridPos.Y + y;

        if (checkX >= 0 && checkX < gridSize.X && checkY >= 0 && checkY < gridSize.Y) {
          neighbours->Add(grid[checkX, checkY]);
        }
      }
    }

    return neighbours;
  }

  Node^ GetNodeFromWorldPoint(Point worldPos) {
    float percentX = (float)worldPos.X / gridWorldSize.X;
    float percentY = (float)worldPos.Y / gridWorldSize.Y;

    percentX = Clamp01(percentX);
    percentY = Clamp01(percentY);

    int x = RoundToInt((gridSize.X - 1) * percentX);
    int y = RoundToInt((gridSize.Y - 1) * percentY);
    return grid[x, y];
  }

  void DrawGizmos(Graphics^ world) {
    this->DrawGizmos(world, Color::Black);
  }

  void DrawGizmos(Graphics^ world, Color color) {
    Pen^ pen = gcnew Pen(color, 1);
    int nodeRadiusX = RoundToInt(nodeRadius.X);
    int nodeRadiusY = RoundToInt(nodeRadius.Y);

    for each (Node ^ node in grid) {
      Point location = Point(node->worldPos.X - nodeRadiusX, node->worldPos.Y - nodeRadiusY);
      Size size = Size(Point::Round(nodeDiameter));
      Rectangle rect = Rectangle(location, size);
      world->DrawRectangle(pen, rect);
    }
  }

  void DrawNodeOutline(Graphics^ world, Point worldPos) {
    Node^ node = GetNodeFromWorldPoint(worldPos);
    SolidBrush^ brush = gcnew SolidBrush(Color::Green);
    Point nodePos = Point(node->worldPos.X - RoundToInt(nodeRadius.X), node->worldPos.Y - RoundToInt(nodeRadius.Y));
    Size size = Size(Point::Round(nodeDiameter));
    Rectangle rect = Rectangle(nodePos, size);
    world->FillRectangle(brush, rect);
  }
};

#endif // !_GRID_H_

