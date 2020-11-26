#pragma once

#ifndef _MAP_H_
#define _MAP_H_

#include "Scene.h"
#include "DataTypes.h"
#include "MathUtils.h"
#include "Pathfinder.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace MathUtils::Mathf;

namespace Defaults {
  namespace Map {
    const double MinScenes = 40;
    const double MaxScenes = 50;
  }
}

ref class Map {
  Scene^ currentScene;
  Random^ rnd;
  Point furthestScene;
  const int seed;
  int maxScenes;
  const MinMax<short>^ depthCount;
  bool isGenerating;
  bool generated;
  bool generateAssassins;

public:
  Map() : Map(RoundToInt(Defaults::Map::MinScenes), RoundToInt(Defaults::Map::MaxScenes), Environment::TickCount) {}

  Map(int seed) : Map(RoundToInt(Defaults::Map::MinScenes), RoundToInt(Defaults::Map::MaxScenes), seed) {}

  Map(int min, int max) : Map(min, max, Environment::TickCount) {}

  Map(int min, int max, int seed) : seed(seed) {
    this->generateAssassins = false;
    this->rnd = gcnew Random(seed);
    this->maxScenes = rnd->Next(min, max);
    int minDepth = RoundToInt(min / (2.0 + 0.1 * min));
    int maxDepth = RoundToInt(max / (2.0 + 0.1 * max));
    this->depthCount = gcnew MinMax<short>(minDepth, maxDepth);
    this->isGenerating = false;
    this->generated = false;
  }

  ~Map() {
    delete currentScene;
    delete rnd;
    delete depthCount;
  }

  void StartGeneration() {
    // Prevent execution if the map has already been generated
    if (generated)
      return;

    // Set the value needed to stop multiple threads from being created
    isGenerating = true;
    // Initialize depth counter
    int depth = 0;
    // Set the position of the first scene in the virtual grid
    Point position = Point(0, 0);
    // Initialize points List (only used to detect collisions)
    Dictionary<Point, int>^ points = gcnew Dictionary<Point, int>;
    // Initialize the first scene
    currentScene = gcnew Scene(DoorLocations(true), position);
    // Set the background of this scene
    currentScene->SetBackground(EnumUtilities::GetRandomBackground(rnd));
    // Add the first Point
    points->Add(position, currentScene->GetHashCode());
    // Create the default spawners
    currentScene->CreateSpawners(points, rnd);
    // Start recursive generation
    GenerateScene(currentScene, points, depth);
    // Finish generation
    generated = true;
    // No longer generating
    isGenerating = false;
  }

  void DrawCurrent(Graphics^ world) {
    if (currentScene == nullptr)
      return;

    currentScene->Draw(world);
  }

  void DrawGizmos(Graphics^ world) {
    // Prevent execution when there's nothing to draw
    if (currentScene == nullptr)
      return;

    // Initialize List to prevent an infinite loop
    List<int>^ drawnNodes = gcnew List<int>;
    DrawSceneGizmos(world, currentScene, drawnNodes);
    // Clear the List and delete
    drawnNodes->Clear();
    delete drawnNodes;
  }

  bool ChangeScene(Direction direction) {
    Scene^ scene;
    if (currentScene->GetNeighbours()->TryGetValue(direction, scene)) {
      currentScene = scene;
      Random rnd;

      if (!currentScene->IsExplored()) {
        currentScene->CreateNPCS(rnd.Next(2), generateAssassins ? rnd.Next(3) : 0);
        currentScene->Explore();
      }

      return true;
    }
    return false;
  }

  void ActivateAsssasins() {
    generateAssassins = true;
  }

  Scene^ GetCurrentScene(){
    return currentScene;
  }

  bool IsGenerated() {
    return this->generated;
  }

  bool IsGenerating() {
    return this->isGenerating;
  }

  bool IsAssassinActivated() {
    return this->generateAssassins;
  }

  int GetSeed() {
    return this->seed;
  }

private:
  void GenerateScene(Scene^ scene, Dictionary<Point, int>^ points, int& depth) {
    // Increase the depth counter
    depth += 1;

    for each (KeyValuePair<Direction, SceneSpawner^> element in scene->GetSpawners()) {
      // Save the reference to the current spawner as a local
      SceneSpawner^ currentSpawner = element.Value;

      // Skip the current spawner since it is nullptr
      if (currentSpawner == nullptr)
        continue;

      // Get the possible position of this scene
      Point position = currentSpawner->GetPos();
      // Get the door direction that the next scene will need
      Direction doorNeeded = currentSpawner->GetParentDirection();

      // Detect possible collisions. Continues to the next
      // loop and deletes the current spawner if a collision
      // was found.
      if (points->ContainsKey(position)) {
        int hashCode;
        points->TryGetValue(position, hashCode);

        if (hashCode != currentSpawner->GetHashCode()) {
          scene->SetDoorValue(EnumUtilities::GetInverseDirection(doorNeeded), false);
          delete currentSpawner;
          continue;
        }
      }

      // Initialize values for the new scene
      DoorLocations doorLocations;
      
      if (points->Count < maxScenes && depth < depthCount->Max) {
        // Set probability
        int probability = 0 + 2 * depth;

        // Set default values
        doorLocations.SetAll(true);

        // Get a random open or closed door
        do {
          if (doorNeeded != Direction::Up)
            doorLocations.Up = rnd->Next(1, 101) > probability;
          if (doorNeeded != Direction::Down)
            doorLocations.Down = rnd->Next(1, 101) > probability;
          if (doorNeeded != Direction::Left)
            doorLocations.Left = rnd->Next(1, 101) > probability;
          if (doorNeeded != Direction::Right)
            doorLocations.Right = rnd->Next(1, 101) > probability;
        } while (GeneratorCondition(doorLocations, doorNeeded, depth, depthCount->Min));
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
      Scene^ generatedScene = gcnew Scene(doorLocations, position);

      // Set the background of this scene
      generatedScene->SetBackground(EnumUtilities::GetRandomBackground(rnd));

      // Create the spawners and add them to the points List
      generatedScene->CreateSpawners(points, rnd, scene, doorNeeded);

      // Remove the spawner from the Dictionary
      points->Remove(position);
      // Save the Point to the Dictionary with the scene hash
      points->Add(position, generatedScene->GetHashCode());

      // Get the distance between this scene and (0, 0)
      // using the same algorithm as for the Pathfinder.
      // Then calculate if this one is more than the current
      // furthest scene.
      if (Pathfinder::GetDistance(Point(0, 0), position) > Pathfinder::GetDistance(Point(0, 0), furthestScene))
        furthestScene = position;

      // Add as a neighbour to the current scene
      scene->AddNeighbour(element.Key, generatedScene);

      // Delete the spawner because the scene has been created
      delete currentSpawner;
      // Continue generating more scenes using recursion
      GenerateScene(generatedScene, points, depth);
    }

    // Clear the List since all the spawners have been used
    scene->GetSpawners()->Clear();
    // Decrease the depth counter
    depth -= 1;
  }

  void DrawSceneGizmos(Graphics^ world, Scene^ scene, List<int>^ drawnNodes) {
    if (drawnNodes->Contains(scene->GetHashCode()))
      return;

    Bitmap^ minimapImage = DoorLocations::GetImage(scene->GetDoorLocations());
    Point position = scene->GetPos();
    Size size = minimapImage->Size;
    Point worldPos = Point((position.X + 10) * size.Width, (position.Y + 10) * size.Height);
    Rectangle rect = Rectangle(worldPos, size);
    
    if (position.Equals(currentScene->GetPos())) {
      world->FillRectangle(Brushes::DeepPink, rect);
    } else if (position.Equals(Point(0, 0))) {
      world->FillRectangle(Brushes::Crimson, rect);
    } else if (position.Equals(furthestScene)) {
      world->FillRectangle(Brushes::BlueViolet, rect);
    } else {
      world->FillRectangle(Brushes::CornflowerBlue, rect);
    }
    world->DrawImage(minimapImage, worldPos);

    drawnNodes->Add(scene->GetHashCode());

    for each (KeyValuePair<Direction, Scene^> element in scene->GetNeighbours()) {
      Scene^ neighbour = element.Value;
      DrawSceneGizmos(world, neighbour, drawnNodes);
    }
  }

  static bool GeneratorCondition(DoorLocations doors, Direction doorNeeded, int depth, int minDepth) {
    bool allTrue = doors.IsAllTrue();
    bool closedScene = false;
    switch (doorNeeded) {
      case Direction::Up:
        closedScene = doors.Up && !doors.Down && !doors.Right && !doors.Left;
        break;
      case Direction::Down:
        closedScene = !doors.Up && doors.Down && !doors.Right && !doors.Left;
        break;
      case Direction::Left:
        closedScene = !doors.Up && !doors.Down && doors.Right && !doors.Left;
        break;
      case Direction::Right:
        closedScene = !doors.Up && !doors.Down && !doors.Right && doors.Left;
        break;
    }
    return allTrue || (closedScene && depth < minDepth);
  }
};

#endif // !_MAP_H_
