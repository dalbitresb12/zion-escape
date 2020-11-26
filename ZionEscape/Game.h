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
#include "MessageBox.h"
#include "Portal.h"

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
  Messagebox^ messagebox;
  List<Portal^>^ portals;
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
  ~Game() {
    delete map;
    delete messagebox;
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

  void Paint(Graphics^ world, Rectangle ClientRectangle) {
    map->DrawCurrent(world);

    if (map != nullptr) {
      DrawMapGizmos(world);
    }

    if (allies != nullptr) {
      for each (Ally ^ ally in allies) {
        ally->Draw(world);

        //If the health of the NPC is 0, it's dead
        if (ally->GetHealth() <= 0) {
          //Delete form the list
          allies->Remove(ally);
          //Delete ptr
          ally = nullptr;
          //The for must be break, beacuse it won't consider the same npc
          break;
        }


      }
    }

    if (player != nullptr) {
      // Add the player to the scene
      player->Draw(world);
      if (map->GetCurrentScene()->GetNPCList() != nullptr)
        player->ActionBullets(world, ClientRectangle, map->GetCurrentScene()->GetNPCList());
      else
        player->ActionBullets(world, ClientRectangle);

      // Draw the player health using the UI controller
      UI::DrawHearts(world, player->GetHealth());
    }

    if (portals != nullptr) {
      for each (Portal ^ portal in portals) {
        portal->Draw(world);
      }
    }

    if (messagebox != nullptr && messagebox->GetActivated()) {
      messagebox->Draw(world);
    }
  }

  void KeyDown(KeyEventArgs^ e) {
    // Temporary map seed viewer
    if (e->KeyCode == Keys::P) {
      Debug::WriteLine("Seed: {0}", GetMapSeed());
      return;
    }

    if (e->KeyCode == Keys::M) {
      map->ActivateAsssasins();
    }

    player->KeyDown(e);
  }

  void KeyUp(KeyEventArgs^ e) {
    player->KeyUp(e);
  }

  void MouseClick(MouseEventArgs^ e) {
     player->Shoot(e->Location.X, e->Location.Y);
  }

  void MovementTick(int tickInterval, Label^ label, Windows::Forms::Timer^ MessageTimer) {
    map->GetCurrentScene()->MoveNPCS(mapGrid, player, tickInterval, allies);

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

    if (this->player->GetHealth() <= 2) {
      //If the list of portals doesnt exist
      if (portals == nullptr) {
        portals = gcnew List<Portal^>;
        //Create the first portal at the position of the player
        portals->Add(gcnew Portal(this->player->GetPosition()));
        //Create the second portal (select the position of the other portal)
        Point pos = Point(300, 200);
        portals->Add(gcnew Portal(pos));
      }
      else {
        for (short i = 0; i < portals->Count; i++)
          if (portals[i]->HasCollision(this->player)) {
            //If the player collides with the portal, its cooldown reduces by 1
            portals[i]->SetCooldown(portals[i]->GetCooldown() - 1);
            //If the cooldown of the portal is 0, the player will be teleported
            if (portals[i]->GetCooldown() <= 0) {
              //Set the position of player to the other portal
              Point pos;
              if (i % 2 == 0)
                pos = portals[++i]->GetPosition();
              else
                pos = portals[--i]->GetPosition();
              //Center the position, so the player will be on the center of the portal
              pos.X -= (this->player->GetCropArea().Size.Width) / 2;
              pos.Y -= (this->player->GetCropArea().Size.Height) / 2;
              //Change the position of the player
              this->player->SetPosition(pos);
            }
          }
          else
            //If the player doesnt collide, the cooldown of the portal is half a second
            portals[i]->SetCooldown(50);
      }
    }

    if (map->IsAssassinActivated() && messagebox == nullptr && label != nullptr) {
      messagebox = gcnew Messagebox(label);
      label->Visible = true;
      MessageTimer->Start();
    }
  }

  void PrintLetterTick(Label^ label, Windows::Forms::Timer^ MessageTimer) {
    messagebox->PrintLetter(label, MessageTimer);
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

    if (portals != nullptr) {
      for each (Portal ^ portal in portals)
        portal->ShiftCol();
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

    if (allies->Count > 0) {
      for each (Ally ^ ally in allies) {
        Node^ currentNode = mapGrid->GetNodeFromWorldPoint(ally->GetPosition());
        if (!playerNeighbours->Contains(currentNode)) {
          Pathfinder::FindPath(mapGrid, ally->GetPosition(), player->GetPosition(), ally);
        }
      }

      map->GetCurrentScene()->ResetPathfinders(mapGrid, playerNeighbours, player->GetPosition(), allies);
    }
  }

  Messagebox^ GetMessagebox() {
    return this->messagebox;
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


