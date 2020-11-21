#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "Map.h"
#include "MessageBox.h"

using namespace Windows::Forms;

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
    if(this->messagebox!=nullptr)
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
    if (this->messagebox != nullptr && this->messagebox->GetActivated())
        this->messagebox->Draw(g);
  }
  
  //Set the message and len
  void SetMessage(Label^ label) {
    this->messagebox->SetText(label->Text);
    this->messagebox->SetLength(this->messagebox->GetText()->Length);
    label->Text = "";
  }

  //Print letter by letter
  void PrintLetter(Label^ label, Timer^ timer) {
    // Time to change to the other message
    short timeNextMessage = 20;

    //Set Color and Interval to Assasins and Corrupts
    if (this->messagebox->GetOrder() == 5) {
      timer->Interval = 50;
      label->ForeColor = Color::LightGreen;
    }
    else if (this->messagebox->GetOrder() % 2) {
      timer->Interval = 50;
      label->ForeColor = Color::LightSteelBlue;
    }
    else {

      if (this->messagebox->GetOrder() != 4)
        timer->Interval = 80;
      //At the last message of the assasin, go faster
      else
        timer->Interval = 20;

      label->ForeColor = Color::Crimson;
    }

    // Counter + 1
    this->messagebox->SetCounter(1);
    //If the counter is greater to the length of the message
    if (this->messagebox->GetCounter() > this->messagebox->GetLength()) {
      timer->Interval = 100;
      // NextMessage +1
      this->messagebox->SetNextMessage(1);
      //If NextMessage reach 30
      if (this->messagebox->GetNextMessage() == timeNextMessage) {

        //If the order is 6, the messagebox will be deleted
        if (this->messagebox->GetOrder() == 5) {
          this->messagebox->SetActivated(false);
          label->Visible = false;
          this->messagebox = nullptr;
          timer->Stop();
          return;
        }

        //NextMessage return to 0
        this->messagebox->SetNextMessage(-timeNextMessage);
        //Counter returns to 0
        this->messagebox->SetCounter(- (this->messagebox->GetLength() + timeNextMessage));
        //Set the new message
        switch (this->messagebox->GetOrder())
        {
        case 0:
          label->Text = "Debemos evitar que salga del laberinto, no podremos\nsolos. Necesitamos de ustedes, asesinos.";
          break;
        case 1:
          label->Text = "Que halagadoras palabras, pero no es suficiente . . .";
          break;
        case 2:
          label->Text = "Obtener el poder absoluto de Zion no es suficiente?\nVan a tener todo lo que ustedes deseen!";
          break;
        case 3:
          label->Text = "Es a donde queria llegar, ja, ja, ja! Hay que asesinarlo!";
          break;
        case 4:
          label->Text = "Los asesinos van a por ti!\nTen cuidado por donde vayas.";
          break;
        }

        this->messagebox->SetOrder(1);
        this->SetMessage(label);
      }
    }
    else {
      //Print a letter of the sring in the label
      label->Text = this->messagebox->GetText()->Substring(0, this->messagebox->GetCounter());
    }
  }
};

#endif // !_GAME_H_


