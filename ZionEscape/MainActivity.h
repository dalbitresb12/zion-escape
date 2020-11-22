#pragma once
#include "Game.h"
#include "BitmapManager.h"
#include "Pathfinder.h"
#include "Grid.h"
#include "Player.h"
#include "Ally.h"
#include "Assassin.h"
#include "Corrupt.h"

namespace ZionEscape {
  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Drawing;
  using namespace System::Collections;
  using namespace System::Data;
  using namespace System::Windows::Forms;
  using namespace System::Diagnostics;

  // Main Activity Form
  public ref class MainActivity : public Form {
    System::ComponentModel::IContainer^ components;
    System::Windows::Forms::Timer^ MovementTimer;
    System::Windows::Forms::Timer^ AnimationTimer;
    // User-defined properties.
    Game^ game;
    Bitmap^ background;
    GraphicsPath^ unwalkableLayer;
    Grid^ mapGrid;
    Player^ player;
    List<NPC^>^ npcs;

    List<Keys>^ keysPressed;
    List<Keys>^ validKeys;

  public:
    MainActivity() {
      // User-defined code.
      BitmapManager^ bmpManager = BitmapManager::GetInstance();
      background = bmpManager->GetImage("assets\\sprites\\scenes\\scene_1.png");
      //Set an icon to the Cursor
      this->Cursor = gcnew System::Windows::Forms::Cursor("assets\\sprites\\misc\\cursor.ico");

      this->game = gcnew Game();

      unwalkableLayer = gcnew GraphicsPath();
      Point gridWorldSize = Point(background->Width, background->Height);
      PointF nodeRadius = PointF(18, 10);
      mapGrid = gcnew Grid(unwalkableLayer, gridWorldSize, nodeRadius);

      player = gcnew Player(Point(200, 400));

      npcs = gcnew List<NPC^>;
      npcs->Add(gcnew Ally(Point(700, 200)));
      npcs->Add(gcnew Ally(Point(450, 200)));
      npcs->Add(gcnew Assassin(Point(300, 100)));
      npcs->Add(gcnew Corrupt(Point(300, 100)));

      ResetPathfinders();

      keysPressed = gcnew List<Keys>;
      validKeys = gcnew List<Keys>;
      validKeys->Add(Keys::W);
      validKeys->Add(Keys::A);
      validKeys->Add(Keys::S);
      validKeys->Add(Keys::D);

      // Important call. Do not delete.
      InitializeComponent();
    }

  protected:
    ~MainActivity() {
      if (components) {
        delete components;
      }
      delete game;
    }

#pragma region Windows Form Designer generated code
    // Required method for Designer support - do not modify the contents of this method with the code editor.
    void InitializeComponent() {
      this->components = (gcnew System::ComponentModel::Container());
      this->MovementTimer = (gcnew System::Windows::Forms::Timer(this->components));
      this->AnimationTimer = (gcnew System::Windows::Forms::Timer(this->components));
      this->SuspendLayout();
      // 
      // MovementTimer
      // 
      this->MovementTimer->Enabled = true;
      this->MovementTimer->Interval = 20;
      this->MovementTimer->Tick += gcnew System::EventHandler(this, &MainActivity::MovementTimer_Tick);
      // 
      // AnimationTimer
      // 
      this->AnimationTimer->Enabled = true;
      this->AnimationTimer->Interval = 80;
      this->AnimationTimer->Tick += gcnew System::EventHandler(this, &MainActivity::AnimationTimer_Tick);
      // 
      // MainActivity
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(936, 624);
      this->DoubleBuffered = true;
      this->Margin = System::Windows::Forms::Padding(4);
      this->Name = L"MainActivity";
      this->Text = L"Zion Escape";
      this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainActivity::MainActivity_Paint);
      this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainActivity::MainActivity_KeyDown);
      this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &MainActivity::MainActivity_KeyUp);
      this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainActivity::MainActivity_MouseClick);
      this->ResumeLayout(false);

    }
#pragma endregion
  private: void MainActivity_Paint(Object^ sender, PaintEventArgs^ e) {
    Graphics^ world = e->Graphics;
    world->DrawImage(this->background, Point(0, 0));
    this->game->StartGeneration(world);
    for each (NPC ^ npc in npcs) {
      npc->Draw(world);
    }

    this->player->Draw(world);
    this->player->ActionBullets(world, this->ClientRectangle, this->npcs);
    this->player->DrawHearts(world);
  }

  private: void MainActivity_KeyDown(Object^ sender, KeyEventArgs^ e) {
    // Temporary Map Seed Print
    if (e->KeyCode == Keys::P) {
      Debug::WriteLine("Seed: {0}", this->game->GetMapSeed());
      return;
    }

    if (!validKeys->Contains(e->KeyCode)) return;

    if (!keysPressed->Contains(e->KeyCode)) {
      keysPressed->Add(e->KeyCode);
      player->StartAnimation();
      ResetPathfinders();
    }
  }

  private: void MainActivity_KeyUp(Object^ sender, KeyEventArgs^ e) {
    if (keysPressed->Contains(e->KeyCode))
      keysPressed->Remove(e->KeyCode);

    if (keysPressed->Count == 0)
      player->StopAnimation();
  }

  private: void MovementTimer_Tick(Object^ sender, EventArgs^ e) {

    for each (NPC ^ npc in npcs) {
      Point deltas = npc->GetDelta();
      npc->Move(deltas.X, deltas.Y);
      //If the health of the NPC is 0, it's dead
      if (npc->GetHealth() == 0) {
        //Delete ptr
        delete npc;
        //Delete form the list
        npcs->Remove(npc);
        //The for must be break, beacuse it won't consider the same npc
        break;
      }
    }

    for each (Keys key in keysPressed) {
      if (!validKeys->Contains(key)) break;
      player->Move(key);
    }
    Refresh();
  }

  private: void ResetPathfinders() {
    //Check if there are npcs
    if (npcs->Count > 0) {
      for each (NPC ^ npc in npcs) {
        if (npc->GetEntityType() == EntityType::Ally || npc->GetEntityType() == EntityType::Assassin) {
          Pathfinder::FindPath(mapGrid, npc->GetPosition(), player->GetPosition(), npc);
        }
        else if (npc->GetEntityType() == EntityType::Corrupt) {
          Random r;
          List<Ally^>^ allies = gcnew List<Ally^>;
          for each (NPC ^ possibleAlly in npcs) {
            if (possibleAlly->GetEntityType() == EntityType::Ally)
              allies->Add((Ally^)possibleAlly);
          }
          Ally^ ally = allies[r.Next(0, allies->Count)];
          Pathfinder::FindPath(mapGrid, npc->GetPosition(), ally->GetPosition(), npc);
        }
      }
    }
  }
  private: System::Void MainActivity_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
    //When you click, the player shoot a bullet in the direction of the mouse
    this->player->Shoot(e->Location.X, e->Location.Y);
  }
  
  private: void AnimationTimer_Tick(Object^ sender, EventArgs^ e) {
    for each (NPC ^ npc in npcs) {
      npc->ShiftCol();
    }

    player->ShiftCol();
  }
};
}
