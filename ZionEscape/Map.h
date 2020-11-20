#pragma once

#ifndef _MAP_H_
#define _MAP_H_

#include "Scene.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class Map {
  // TO DO: Remove scenes List and use neighbours approach
  List<Scene^>^ scenes;
  Random^ rnd;
  int seed;
  int maxScenes;
  bool isGenerating;
  bool generated;

public:
  Map() : Map(40, 50, Environment::TickCount) {}

  Map(int seed) : Map(40, 50, seed) {}

  Map(int min, int max) : Map(min, max, Environment::TickCount) {}

  Map(int min, int max, int seed) {
    this->seed = seed;
    this->rnd = gcnew Random(seed);
    this->maxScenes = rnd->Next(min, max);
    this->Reset();
  }

  ~Map() {
    for each (Scene ^ scene in this->scenes)
      delete scene;
    this->scenes->Clear();
    delete this->scenes;
    delete this->rnd;
  }

  void Reset() {
    if (this->scenes != nullptr) {
      for each (Scene ^ scene in this->scenes)
        delete scene;
      this->scenes->Clear();
      delete this->scenes;
    }

    // Set the values of generation
    this->isGenerating = false;
    this->generated = false;

    // Create the List of scenes and select the start point of the first one
    this->scenes = gcnew List<Scene^>;
    this->scenes->Add(gcnew Scene(DoorLocations(true), Point(0, 0)));
  }

  void StartGeneration() {
    // Generate new Scenes
    if (isGenerating) {
      // Check all the scenes
      for (int sceneCounter = 0; sceneCounter < scenes->Count; ++sceneCounter) {
        // Save the current scene as a local
        Scene^ currentScene = scenes[sceneCounter];
        // Number of spawners in the current scene
        int spawnersCount = currentScene->GetSpawners()->Count;

        if (spawnersCount > 0) {
          isGenerating = true;

          // For each spawner of the scene we need to know if it collides or not
          for each (KeyValuePair<Direction, SceneSpawner^> element in currentScene->GetSpawners()) {
            // Save the reference to the current spawner as a local
            SceneSpawner^ currentSpawner = element.Value;

            // Skip the current spawner since it is nullptr
            if (currentSpawner == nullptr)
              continue;

            // Get the door direction that the next scene will need
            Direction doorNeeded = currentSpawner->GetParentDirection();
            bool spawnerCollides = false;

            // Check all scenes for possible collisions
            for each (Scene ^ scene in scenes) {
              // Check all spawners of the scene for possible collisions
              for each (KeyValuePair<Direction, SceneSpawner^> element in scene->GetSpawners()) {
                SceneSpawner^ spawner = element.Value;
                if (!currentSpawner->Equals(spawner)) {
                  // If the current spawner is in the same position of another spawner
                  // a door of the current scene should be closed
                  if (currentSpawner->GetPos().Equals(spawner->GetPos())) {
                    spawnerCollides = true;
                    CloseDoor(doorNeeded, currentScene);
                    break;
                  }
                }
              }

              // Exit if already found
              if (spawnerCollides)
                break;

              // If the current spawner is in the same position of another scene
              // a door of the current scene should be closed
              if (currentSpawner->GetPos().Equals(scene->GetPos())) {
                spawnerCollides = true;
                CloseDoorScene(doorNeeded, currentScene, scene);
              }
            }

            // Continue to next loop and delete the current spawner
            if (spawnerCollides) {
              delete currentSpawner;
              continue;
            }

            // Initialize values for the new scene
            DoorLocations doorLocations;

            if (scenes->Count < maxScenes) {
              // Set default values
              doorLocations.SetAll(true);
              // Get a random open or closed door
              do {
                if (doorNeeded != Direction::Up)
                  doorLocations.Up = rnd->Next(0, 2);
                if (doorNeeded != Direction::Down)
                  doorLocations.Down = rnd->Next(0, 2);
                if (doorNeeded != Direction::Left)
                  doorLocations.Left = rnd->Next(0, 2);
                if (doorNeeded != Direction::Right)
                  doorLocations.Right = rnd->Next(0, 2);
              } while (doorLocations.IsAllTrue());
            } else {
              // If the number of scenes reaches the max scenes,
              // the current scene needs to be closed
              switch (doorNeeded) {
                case Direction::Up:
                  doorLocations.SetAll(true, false, false, false);
                  break;
                case Direction::Down:
                  doorLocations.SetAll(false, true, false, false);
                  break;
                case Direction::Left:
                  doorLocations.SetAll(false, false, true, false);
                  break;
                case Direction::Right:
                  doorLocations.SetAll(false, false, false, true);
                  break;
              }
            }

            // Create the new scene
            Scene^ scene = gcnew Scene(doorLocations, currentSpawner->GetPos());
            scenes->Add(scene);
            // Add as a neighbour to the current scene
            currentScene->AddNeighbour(EnumUtilities::GetInverseDirection(doorNeeded), scenes[scenes->Count - 1]);
            // Delete the spawner because the scene has been created
            delete currentSpawner;
          }

          // Clear the List since all the spawners have been used
          currentScene->GetSpawners()->Clear();
        } else {
          isGenerating = false;
        }
      }
    } else if (scenes->Count == 1) {
      isGenerating = true;
    } else if (scenes->Count < maxScenes && !isGenerating) {
      Reset();
    } else {
      // The map has been generated succesfully.
      generated = true;
    }
  }

  void DrawScenes(Graphics^ world) {
    DrawScenes(world, false);
  }

  void DrawScenes(Graphics^ world, bool background) {
    if (background) {
      // Put a color to the background
      world->Clear(Color::FromArgb(255, 37, 37, 37));
    }

    // Draw all the scenes
    for each (Scene ^ scene in scenes) {
      // Put a color to the maze
      Point position = scene->GetPos();
      Size size = scene->GetBackgroundSize();
      Point worldPos = Point((position.X + 10) * size.Width, (position.Y + 10) * size.Height);
      Rectangle rect = Rectangle(worldPos, size);
      world->FillRectangle(Brushes::CornflowerBlue, rect);

      // Put a different color to the background of the first and last scene
      if (scene == scenes[0])
        world->FillRectangle(Brushes::Crimson, rect);
      else if (scene == scenes[scenes->Count - 1])
        world->FillRectangle(Brushes::BlueViolet, rect);

      // Draw the scene
      scene->Draw(world);
    }
  }

  bool IsGenerated() {
    return this->generated;
  }

  int GetSeed() {
    return this->seed;
  }

private:
  static void CloseDoor(Direction doorNeeded, Scene^ scene) {
    scene->SetDoorValue(EnumUtilities::GetInverseDirection(doorNeeded), false);
  }

  static void CloseDoorScene(Direction doorNeeded, Scene^ sceneA, Scene^ sceneB) {
    if (!sceneB->GetDoorValue(doorNeeded)) {
      sceneA->SetDoorValue(EnumUtilities::GetInverseDirection(doorNeeded), false);
    }
  }
};

#endif // !_MAP_H_
