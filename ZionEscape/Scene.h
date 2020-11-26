#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "SceneSpawner.h"
#include "BitmapManager.h"
#include "Enums.h"
#include "DataTypes.h"
#include "NPC.h"
#include "Grid.h"
#include "Player.h"
#include "Ally.h"
#include "Corrupt.h"
#include "Assassin.h"

using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Scene {
  Bitmap^ background;
  List<Direction>^ doors;
  List<NPC^>^ npcs;
  Dictionary<Direction, Scene^>^ neighbours;
  Dictionary<Direction, SceneSpawner^>^ spawners;
  bool explored;
  // Only used to check collisions
  Point position;

public:
  Scene(DoorLocations doorLocations, Point position) {
    this->InitDoorsList(doorLocations);
    this->neighbours = gcnew Dictionary<Direction, Scene^>;
    this->spawners = gcnew Dictionary<Direction, SceneSpawner^>;
    this->position = position;
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

  void Draw(Graphics^ world) {
    Draw(world, Point(0, 0));
  }

  void Draw(Graphics^ world, Point position) {
    // Prevent execution if there's no background to draw
    if (background == nullptr)
      return;

    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ door = bmpManager->GetImage("assets\\sprites\\misc\\door.png");
    DoorLocations locations = GetDoorLocations();

    world->DrawImage(background, position);

    if (locations.Up) {
      world->DrawImage(door, Point(422, 27));
    }

    if (locations.Down) {
      door->RotateFlip(RotateFlipType::Rotate180FlipNone);
      world->DrawImage(door, Point(422, 520));
      door->RotateFlip(RotateFlipType::Rotate180FlipNone);
    }

    if (locations.Left) {
      door->RotateFlip(RotateFlipType::Rotate270FlipNone);
      world->DrawImage(door, Point(27, 266));
      door->RotateFlip(RotateFlipType::Rotate90FlipNone);
    }

    if (locations.Right) {
      door->RotateFlip(RotateFlipType::Rotate90FlipNone);
      world->DrawImage(door, Point(831, 266));
      door->RotateFlip(RotateFlipType::Rotate270FlipNone);
    }

    if (npcs != nullptr) {
      for each (NPC ^ npc in npcs) {
        npc->Draw(world);

        //If the health of the NPC is 0, it's dead
        if (npc->GetHealth() <= 0) {
          //Delete form the list
          npcs->Remove(npc);
          //Delete ptr
          npc = nullptr;
          //The for must be break, beacuse it won't consider the same npc
          break;
        }
      }
    }
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

  void SetBackground(BackgroundImage image) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    background = bmpManager->GetImage(EnumUtilities::GetPathFromBackground(image));
  }

  void Explore() {
    explored = true;
  }

  void SetDoorValue(Direction direction, bool value) {
    if (value && !doors->Contains(direction))
      doors->Add(direction);
    else if (!value && doors->Contains(direction))
      doors->Remove(direction);
  }

  bool IsExplored() {
    return explored;
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

  List<NPC^>^ GetNPCList() {
    return npcs;
  }

  void CreateNPCS(int corrupts, int assassins) {
    if (npcs == nullptr) {
      npcs = gcnew List<NPC^>;
    }

    Random rnd;
    for (int i = 0; i < corrupts; ++i) {
      int x = rnd.Next(143, 858);
      int y = rnd.Next(143, 520);
      Corrupt^ entity = gcnew Corrupt(Point(x, y));
      npcs->Add(entity);
    }

    for (int i = 0; i < assassins; ++i) {
      int x = rnd.Next(143, 858);
      int y = rnd.Next(143, 520);
      Assassin^ entity = gcnew Assassin(Point(x, y));
      npcs->Add(entity);
    }
  }

  void MoveNPCS(Grid^ grid, Player^ player, int tickInterval, List<Ally^>^ allies) {
    // Prevent execution if NPCs is nullptr
    // This will prevent the worst error in the debugger, ever:
    // 'System.NullReferenceException' occurred in Unknown Module
    // Don't suffer what I suffered. Don't forget to check this first.
    if (npcs != nullptr) {
      for each (NPC ^ npc in npcs) {
        Point deltas = npc->GetDelta();
        npc->Move(deltas.X, deltas.Y, grid);

        // Enemy Damage - If the NPC is an assassin
        if (npc->GetEntityType() == EntityType::Assassin) {
          // Reference the assassin
          Assassin^ assassin = (Assassin^)npc;
          // And its cooldown is equal or less than 0
          if (assassin->GetCooldown() <= 0) {
            // If the assasin collides with the player (The health of the player must be greater than 0)
            if (player->HasCollision(assassin) && player->GetHealth() > 0.f) {
              // Assasin damages the player
              player->SetHealth(player->GetHealth() - assassin->GetDamagePoints());
              // And its cooldown will be half a second to attack again
              assassin->SetCooldown(500 / tickInterval);
            }
          } else {
            // Each tick the cooldown will be reduced if it's greater than 0
            assassin->SetCooldown(assassin->GetCooldown() - 1);
          }
        }

        //Corrupt convertion - If the NPC is a Corrupt
        if (npc->GetEntityType() == EntityType::Corrupt) {
          //Looking for the allies
          for each (Ally ^ ally in allies)
              //If the corrupts collides with an ally,
              if (npc->HasCollision((NPC^)ally)) {
                //Reference the corrupt
                Corrupt^ corrupt = (Corrupt^)npc;
                //Conver the corrup to a fake ally
                corrupt->ConvertToAlly();
                //And the health points of the ally are reduced
                ally->SetHealth(ally->GetHealth() - npc->GetDamagePoints());
              }
        }
      }
    }
  }

  void AnimateNPCS() {
    if (npcs != nullptr) {
      for each (NPC ^ npc in npcs) {
        npc->ShiftCol();
      }
    }
  }

  void ResetPathfinders(Grid^ grid, List<Node^>^ playerNeighbours, Point playerPos, List<Ally^>^ allies) {
    // Prevent execution if NPCs is nullptr
    // This will prevent the worst error in the debugger, ever:
    // 'System.NullReferenceException' occurred in Unknown Module
    // Don't suffer what I suffered. Don't forget to check this first.
    if (npcs == nullptr)
      return;

    for each (NPC ^ npc in npcs) {
      if (npc->GetEntityType() == EntityType::Assassin) {
        Node^ currentNode = grid->GetNodeFromWorldPoint(npc->GetPosition());
        if (!playerNeighbours->Contains(currentNode)) {
          Pathfinder::FindPath(grid, npc->GetPosition(), playerPos, npc);
        }
      } else if (npc->GetEntityType() == EntityType::Corrupt) {
        Corrupt^ corrupt = (Corrupt^)npc;

      if(allies->Count>0)
        if (corrupt->tracking == nullptr) {
          Random r;
          corrupt->tracking = allies[r.Next(0, allies->Count)];
        }

        Node^ allyNode = grid->GetNodeFromWorldPoint(corrupt->tracking->GetPosition());
        List<Node^>^ allyNeighbours = grid->GetNeighbours(allyNode);
        Node^ currentNode = grid->GetNodeFromWorldPoint(npc->GetPosition());
        if (!allyNeighbours->Contains(currentNode)) {
          Pathfinder::FindPath(grid, corrupt->GetPosition(), corrupt->tracking->GetPosition(), npc);
        }
      }
    }
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


