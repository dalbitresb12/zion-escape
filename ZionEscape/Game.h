#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "UI.h"
#include "Map.h"
#include "Grid.h"
#include "Pathfinder.h"
#include "NPC.h"
#include "Ally.h"
#include "Assassin.h"
#include "Corrupt.h"
#include "Player.h"
#include "Enums.h"
#include "Scene.h"
#include "BitmapManager.h"
#include "DataTypes.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Threading;
using namespace System::Threading::Tasks;
// TO DO: Remove when UI to show the map seed is added
using namespace System::Diagnostics;

ref class Game {
  Map^ map;
  Grid^ mapGrid;
  Player^ player;
  List<Ally^>^ allies;
  bool initialized;

public:
  Game() {
    map = gcnew Map();
    initialized = false;
  }

  void Init(Size ClientSize) {
    player = gcnew Player(Point(ClientSize.Width / 2, ClientSize.Height / 2));
    InitAllies();
    InitGrid(ClientSize);
    initialized = true;
    ResetPathfinders();
  }

  bool HasInitialized() {
    return initialized;
  }

  ~Game(){
    delete this->map;
  }

  bool IsGenerated() {
    return this->map->IsGenerated();
  }

  int GetMapSeed() {
    return this->map->GetSeed();
  }

  void DrawMapGizmos(Graphics^ world) {
    if (this->map->IsGenerated())
      this->map->DrawGizmos(world);
  }

  void MapGeneration() {
    if (!map->IsGenerating() && !map->IsGenerated()) {
      Action^ action = gcnew Action(map, &Map::StartGeneration);
      Task^ generator = Task::Factory->StartNew(action, TaskCreationOptions::LongRunning);
    }
  }

  void Paint(Graphics^ world) {
    map->DrawCurrent(world);

    if (map != nullptr) {
      DrawMapGizmos(world);
    }

    if (allies != nullptr) {
      for each (Ally ^ ally in allies) {
        ally->Draw(world);
      }
    }

    if (player != nullptr) {
      // Add the player to the scene
      player->Draw(world);

      // Draw the player health using the UI controller
      UI::DrawHearts(world, player->GetHealth());
    }
  }

  void KeyDown(KeyEventArgs^ e) {
    // Temporary map seed viewer
    if (e->KeyCode == Keys::P) {
      Debug::WriteLine("Seed: {0}", GetMapSeed());
      return;
    }

    player->KeyDown(e);
  }

  void KeyUp(KeyEventArgs^ e) {
    player->KeyUp(e);
  }

  void MovementTick(int tickInterval) {
    map->GetCurrentScene()->MoveNPCS(mapGrid, player, tickInterval);

    if (allies != nullptr) {
      for each (Ally ^ ally in allies) {
        Point deltas = ally->GetDelta();
        ally->Move(deltas.X, deltas.Y, mapGrid);
      }
    }

    if (player != nullptr) {
      if (player->MoveUsingKeysList(mapGrid->walkableLayer)) {
        BitmapManager^ bmpManager = BitmapManager::GetInstance();
        Point position = player->GetPosition();
        Size size = player->GetSize();
        bool sceneChanged = false;

        // Up
        if (position.Y < 78) {
          map->ChangeScene(Direction::Up);
          player->SetPosition(position.X - size.Width / 2, 519 - size.Height);
          sceneChanged = true;
        }
        // Down
        if (position.Y > 536) {
          map->ChangeScene(Direction::Down);
          player->SetPosition(position.X - size.Width / 2, 104);
          sceneChanged = true;
        }
        // Left
        if (position.X < 78) {
          map->ChangeScene(Direction::Left);
          player->SetPosition(830 - size.Width, position.Y - size.Height / 2);
          sceneChanged = true;
        }
        // Right
        if (position.X > 850) {
          map->ChangeScene(Direction::Right);
          player->SetPosition(104, position.Y - size.Height / 2);
          sceneChanged = true;
        }


        if (sceneChanged) {
          // O^2 = Too slow, too much CPU
          // Didn't have time to optimize it
          mapGrid->UpdateNodes(GetWalkableLayer(map->GetCurrentScene()));
          // This one is faster
          // mapGrid->walkableLayer = GetWalkableLayer(map->GetCurrentScene());
        }
      }
    }
  }

  void AnimationTick() {
    map->GetCurrentScene()->AnimateNPCS();

    if (allies != nullptr) {
      for each (Ally ^ ally in allies) {
        ally->ShiftCol();
      }
    }

    if (player != nullptr) {
      player->ShiftCol();
    }
  }

  void ResetPathfinders() {
    // Prevent execution if NPCs is nullptr
    // This will prevent the worst error in the debugger, ever:
    // 'System.NullReferenceException' occurred in Unknown Module
    // Don't suffer what I suffered. Don't forget to check this first.
    if (allies == nullptr)
      return;

    Node^ playerNode = mapGrid->GetNodeFromWorldPoint(player->GetPosition());
    List<Node^>^ playerNeighbours = mapGrid->GetNeighbours(playerNode);

    for each (Ally ^ ally in allies) {
      Node^ currentNode = mapGrid->GetNodeFromWorldPoint(ally->GetPosition());
      if (!playerNeighbours->Contains(currentNode)) {
        Pathfinder::FindPath(mapGrid, ally->GetPosition(), player->GetPosition(), ally);
      }
    }

    map->GetCurrentScene()->ResetPathfinders(mapGrid, playerNeighbours, player->GetPosition(), allies);
  }

private:
  static GraphicsPath^ GetWalkableLayer(Scene^ scene) {
    GraphicsPath^ walkableLayer = gcnew GraphicsPath();
    walkableLayer->AddRectangle(Rectangle(Point(103, 103), Size(728, 417)));

    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ doorImage = bmpManager->GetImage("assets\\sprites\\misc\\door.png");
    int width = doorImage->Width;
    int height = doorImage->Height;
 
    DoorLocations doorLocations = scene->GetDoorLocations();
    if (doorLocations.Up) {
      walkableLayer->AddRectangle(Rectangle(Point(422, 27), Size(width, height)));
    }
    if (doorLocations.Down) {
      walkableLayer->AddRectangle(Rectangle(Point(422, 520), Size(width, height)));
    }
    if (doorLocations.Left) {
      walkableLayer->AddRectangle(Rectangle(Point(27, 266), Size(height, width)));
    }
    if (doorLocations.Right) {
      walkableLayer->AddRectangle(Rectangle(Point(831, 266), Size(height, width)));
    }

    return walkableLayer;
  }

  void InitGrid(Size ClientSize) {
    Point gridWorldSize = Point(ClientSize);
    PointF nodeRadius = PointF(18, 10);

    mapGrid = gcnew Grid(GetWalkableLayer(map->GetCurrentScene()), gridWorldSize, nodeRadius);
  }

  void InitAllies() {
    allies = gcnew List<Ally^>;
    allies->Add(gcnew Ally(Point(700, 200)));
    allies->Add(gcnew Ally(Point(450, 200)));
  }
};

#endif // !_GAME_H_


