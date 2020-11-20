#pragma once

#ifndef _MAP_H_
#define _MAP_H_

#include "Scene.h"
#include "Player.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Drawing;

ref class Map {
  List<Scene^>^ scenes;
  Random^ rnd;
  Bitmap^ doorImage;
  Size screenArea;
  int seed;
  int maxScenes;
  bool isGenerating;
  bool generated;

public:
  Map(Size screenArea): Map(40, 50, Environment::TickCount, screenArea) {}

  Map(int seed, Size screenArea): Map(40, 50, seed, screenArea) {}

  Map(int min, int max, Size screenArea): Map(min, max, Environment::TickCount, screenArea) {}

  Map(int min, int max, int seed, Size screenArea) {
    this->screenArea = screenArea;
    this->seed = seed;
    this->rnd = gcnew Random(seed);
    this->maxScenes = rnd->Next(min, max);
    this->Reboot();
  }

  ~Map() {
    for each (Scene ^ scene in this->scenes)
      delete scene;
    this->scenes->Clear();
    delete this->scenes;
    delete this->rnd;
  }

  void Reboot() {
    if (this->scenes != nullptr) {
      for each (Scene^ scene in this->scenes)
        delete scene;
      this->scenes->Clear();
      delete this->scenes;
    }

    //Turn the values of generation to false
    this->isGenerating = false;
    this->generated = false;

    //Create the list of scenes and select the start point of the first one
    this->scenes = gcnew List<Scene^>;
    this->CreateScene(true, true, true, true, Point(0, 0));
  }

  void CreateScene(bool up, bool down, bool left, bool right, Point pos) {
    int bg = rnd->Next(0, 4);
    this->scenes->Add(gcnew Scene(up, down, left, right, bg, pos, this->screenArea));
    this->scenes[this->scenes->Count - 1]->CreateSpawner(pos);
  }

  void StartGeneration(Graphics^ g, Player^ player) {
    //Generate new Scenes
    if (this->isGenerating) {

      //Check all the scenes
      for (short curScene = 0; curScene < this->scenes->Count; curScene++) {
        //Number of spawners in the current Scene
        short countSpawners = this->scenes[curScene]->GetSpawners()->Count;

        if(countSpawners > 0) {

          this->isGenerating = true;

          //For each Spawner of the Scene we need to know if they collides or not
          for (short curSpawner = countSpawners; curSpawner > 0; curSpawner--) {
            //Get the door direction that the next scene will need
            Direction doorNeeded = this->scenes[curScene]->GetSpawners()[curSpawner - 1]->GetParentDirection();
            bool spawnerCollides = false;

            //Check all scenes
            for each (Scene ^ otherScene in this->scenes) {
              //Check all ScenesSpawners of the otherScene
              for each (SceneSpawner ^ otherSpawner in otherScene->GetSpawners()) {
                if (this->scenes[curScene]->GetSpawners()[curSpawner - 1] != otherSpawner) {
                  //If the curSpawner is in the same position of otherSpawner, a door of the curScene will be closed
                  if (this->scenes[curScene]->GetSpawners()[curSpawner - 1]->GetPos().Equals(otherSpawner->GetPos())) {
                    spawnerCollides = true;
                    this->CloseDoor(doorNeeded, this->scenes[curScene]);
                    break;
                  }
                }
              }
              if (spawnerCollides)
                break;
              //If the curSpawner is in the same position of otherScene, a door of the curScene will be closed
              if (this->scenes[curScene]->GetSpawners()[curSpawner - 1]->GetPos().Equals(otherScene->GetPos())) {
                spawnerCollides = true;
                this->CloseDoorScene(doorNeeded, this->scenes[curScene], otherScene);
                break;
              }
            }

            if (spawnerCollides) {
              this->scenes[curScene]->DeleteSpawner(curSpawner - 1);
              break;
            }

            //Values of the new scene
            bool up = true, down = true, right = true, left = true;
            Point pos = this->scenes[curScene]->GetSpawners()[curSpawner - 1]->GetPos();
            
            if (this->scenes->Count < this->maxScenes) {
              //Get a random open or close door
              do {
                if (doorNeeded != Direction::Up)
                  up = rnd->Next(0, 2);
                if (doorNeeded != Direction::Down)
                  down = rnd->Next(0, 2);
                if (doorNeeded != Direction::Right)
                  right = rnd->Next(0, 2);
                if (doorNeeded != Direction::Left)
                  left = rnd->Next(0, 2);
              } while (up == down == right == left == true);
            }
            //If number of scenes reaches the maxScenes, the current scene needs to be closed
            else {
              switch (doorNeeded)
              {
              case Direction::Up:
                down = right = left = false;
                break;
              case Direction::Down:
                up = right = left = false;
                break;
              case Direction::Right:
                up = down = left = false;
                break;
              case Direction::Left:
                up = down = right = false;
                break;
              }
            }
            //Create a new scene
            this->CreateScene(up, down, left, right, pos);
            //After the creation of the scene, delete the curSpawner
            this->scenes[curScene]->DeleteSpawner(curSpawner - 1);
          }
        }
        else {
          this->isGenerating = false;
        }

      }
    }
    else if (this->scenes->Count == 1) {
      this->isGenerating = true;
    }
    else if (this->scenes->Count < this->maxScenes && this->isGenerating == false) {
      this->Reboot();
    }
    else {
      //The Map is now generated
      this->generated = true;
      //Draw all the scenes
      for each (Scene ^ curScene in this->scenes) {
        // Draw the scene
        if (player->Collides(curScene->GetDrawingArea())) {
          //Draw the scene
          this->DrawScene(g, curScene);
          //Draw their doors
          this->DrawDoors(g, curScene);
        }
      }
    }
  }

  void DrawScene(Graphics^ g, Scene^ scene) {

    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    //If you want, you can select, in the switch, a door depends of the scene
    switch (scene->GetBackground())
    {
    case 0:
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\scene_1.png"), Point(0, 0));
      break;
    case 1:
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\scene_2.png"), Point(0, 0));
      break;
    case 2:
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\scene_3.png"), Point(0, 0));
      break;
    case 3:
      g->DrawImage(bmpManager->GetImage("assets\\sprites\\scenes\\scene_4.png"), Point(0, 0));
      break;
    }
  }

  void DrawDoors(Graphics^ g, Scene^ scene) {

    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    //Select an image to the door
    this->doorImage = bmpManager->GetImage("assets\\sprites\\scenes\\doors\\door1.png");

    if (scene->GetUp()) {
      //Draw the image
      g->DrawImage(doorImage, Point(422, 27));
    }

    if (scene->GetRight()) {
      //Image is rotated
      doorImage->RotateFlip(RotateFlipType::Rotate90FlipNone);
      g->DrawImage(doorImage, Point(831, 266));
      //Image returns to its original angle
      doorImage->RotateFlip(RotateFlipType::Rotate270FlipNone);
    }

    if (scene->GetDown()) {
      //Image is rotated
      doorImage->RotateFlip(RotateFlipType::Rotate180FlipNone);
      g->DrawImage(doorImage, Point(422, 520));
      doorImage->RotateFlip(RotateFlipType::Rotate180FlipNone);
    }
    
    if (scene->GetLeft()) {
      //Image is rotated
      doorImage->RotateFlip(RotateFlipType::Rotate270FlipNone);
      g->DrawImage(doorImage, Point(27, 266));
      //Image returns to its original angle
      doorImage->RotateFlip(RotateFlipType::Rotate90FlipNone);
    }
  }

  void MoveMap(Player^ player) {

    bool changeposition = false;
    //Position to add to the scenes
    Point add(0, 0);

    //If the player goes to the up door
    if (player->GetPosition().Y <= 60) {
      player->SetPosition(Point(446, 460));
      add.Y = this->screenArea.Height;
      changeposition = true;
    }
    //If the player goes to the down door
    else if (player->GetPosition().Y >= 530) {
      player->SetPosition(Point(446, 80));
      add.Y = -this->screenArea.Height;
      changeposition = true;
    }
    //If the player goes to the left door
    else if (player->GetPosition().X <= 70) {
      player->SetPosition(Point(795, 266));
      add.X = this->screenArea.Width;
      changeposition = true;
    }
    //If the player goes to the right door
    else if (player->GetPosition().X >= 850) {
      player->SetPosition(Point(90, 266));
      add.X = -this->screenArea.Width;
      changeposition = true;
    }
    //Did the player go to a door?
    if (changeposition) {
      //Change all the scenes position
      for each (Scene ^ scene in this->scenes) {
        scene->SetPosition(Point(scene->GetPos().X + add.X, scene->GetPos().Y + add.Y));
      }
    }
  }

  void CloseDoor(Direction doorNeeded, Scene^ scene) {
    switch (doorNeeded)
    {
    case Direction::Up:
      scene->SetDown(false);
      break;
    case Direction::Down:
      scene->SetUp(false);
      break;
    case Direction::Right:
      scene->SetLeft(false);
      break;
    case Direction::Left:
      scene->SetRight(false);
      break;
    }
  }

  void CloseDoorScene(Direction doorNeeded, Scene^ scene, Scene^ otherScene) {
    switch (doorNeeded)
    {
    case Direction::Up:
      if (!otherScene->GetUp())
        scene->SetDown(false);
      break;
    case Direction::Down:
      if (!otherScene->GetDown())
        scene->SetUp(false);
      break;
    case Direction::Right:
      if (!otherScene->GetRight())
        scene->SetLeft(false);
      break;
    case Direction::Left:
      if (!otherScene->GetLeft())
        scene->SetRight(false);
      break;
    }
  }

  bool IsGenerated() {
    return this->generated;
  }

  int GetSeed() {
    return this->seed;
  }
};

#endif // !_MAP_H_
