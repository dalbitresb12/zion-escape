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
    // User-defined properties.
    Game^ game;
    Bitmap^ background;
    GraphicsPath^ unwalkableLayer;
    Grid^ mapGrid;
    Player^ player;
    List<NPC^>^ npcs;

    List<Keys>^ keysPressed;
  private: System::Windows::Forms::Label^ MessageLabel;

  private: System::Windows::Forms::Timer^ MessageTimer;

  List<Keys>^ validKeys;

  public:
    MainActivity() {
      // User-defined code.
      BitmapManager^ bmpManager = BitmapManager::GetInstance();
      background = bmpManager->GetImage("assets\\sprites\\scenes\\scene_1.png");

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
      this->MessageLabel = (gcnew System::Windows::Forms::Label());
      this->MessageTimer = (gcnew System::Windows::Forms::Timer(this->components));
      this->SuspendLayout();
      // 
      // MovementTimer
      // 
      this->MovementTimer->Enabled = true;
      this->MovementTimer->Interval = 30;
      this->MovementTimer->Tick += gcnew System::EventHandler(this, &MainActivity::MovementTimer_Tick);
      // 
      // MessageLabel
      // 
      this->MessageLabel->AutoSize = true;
      this->MessageLabel->BackColor = System::Drawing::Color::Transparent;
      this->MessageLabel->Font = (gcnew System::Drawing::Font(L"High Tower Text", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(0)));
      this->MessageLabel->ForeColor = System::Drawing::Color::Black;
      this->MessageLabel->Location = System::Drawing::Point(144, 270);
      this->MessageLabel->Name = L"MessageLabel";
      this->MessageLabel->Size = System::Drawing::Size(417, 32);
      this->MessageLabel->TabIndex = 0;
      this->MessageLabel->Text = L"Conque buscan nuestra ayuda . . .";
      this->MessageLabel->Visible = false;
      // 
      // MessageTimer
      // 
      this->MessageTimer->Tick += gcnew System::EventHandler(this, &MainActivity::MessageTimer_Tick);
      // 
      // MainActivity
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(936, 624);
      this->Controls->Add(this->MessageLabel);
      this->DoubleBuffered = true;
      this->Name = L"MainActivity";
      this->Load += gcnew System::EventHandler(this, &MainActivity::MainActivity_Load);
      this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainActivity::MainActivity_Paint);
      this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainActivity::MainActivity_KeyDown);
      this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &MainActivity::MainActivity_KeyUp);
      this->ResumeLayout(false);
      this->PerformLayout();

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

    //This have to be in front of everything, because the message box have to be in front of any image
    this->game->DrawMessagebox(world);

  }

  private: void MainActivity_KeyDown(Object^ sender, KeyEventArgs^ e) {
    //Temporary Message Box
    if (e->KeyCode == Keys::M) {
      this->game->StartMessagebox();
      this->game->SetMessage(this->MessageLabel);
      this->MessageTimer->Start();
      return;
    }

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
    }

    for each (Keys key in keysPressed) {
      if (!validKeys->Contains(key)) break;
      player->Move(key);
    }
    Refresh();
  }

  private: void ResetPathfinders() {
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
  private: System::Void MainActivity_Load(System::Object^ sender, System::EventArgs^ e) {
}
  private: System::Void MessageTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
    this->MessageLabel->Visible = true;
    this->game->PrintLetter(this->MessageLabel, this->MessageTimer);
  }
};
}
