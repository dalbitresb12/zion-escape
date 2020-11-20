#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "Map.h"
#include "MessageBox.h"

ref class Game {
  Map^ map;
  Messagebox^ messagebox;

public:
  Game() {
    this->map = gcnew Map();
    this->messagebox = gcnew Messagebox();
  }

  ~Game(){
    delete this->map;
    delete this->messagebox;
  }

  void StartGeneration(Graphics^ g) {
    this->map->StartGeneration(g);
  }

  bool IsGenerated() {
    return this->map->IsGenerated();
  }

  int GetMapSeed() {
    return this->map->GetSeed();
  }



  //MessageBox
  //Start the message box
  void StartMessagebox() {
    this->messagebox->SetActivated(true);
  }

  //Draw the Message
  void DrawMessagebox(Graphics^ g) {
    if (this->messagebox->GetActivated()) {
      this->messagebox->Draw(g);
      
    }
  }
  
  //Set the message and len
  void SetMessage(System::Windows::Forms::Label^ label) {
    this->messagebox->SetTxt(label->Text);
    this->messagebox->SetLen(this->messagebox->GetTxt()->Length);
    label->Text = "";
  }

  void PrintLetter(System::Windows::Forms::Label^ label) {
    this->messagebox->SetCounter(1);
    if (this->messagebox->GetCounter() > this->messagebox->GetLen()) {
      this->messagebox->SetCounter(0);
      this->messagebox->SetTxt("");
    }
    else {
      label->Text = this->messagebox->GetTxt()->Substring(0, this->messagebox->GetCounter());
    }
    if (label->ForeColor == Color::White)
      label->ForeColor = Color::Red;
    else
      label->ForeColor = Color::White;
  }


};

#endif // !_GAME_H_


