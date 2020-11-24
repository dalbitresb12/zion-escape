#pragma once

#ifndef _MESSAGEBOX_H_
#define _MESSAGEBOX_H_

#include "BitmapManager.h"

ref class Messagebox {
  bool activated;
  unsigned int counter, length, order, nextMessage;
  String^ text;
public:
  Messagebox(Label^ label) {
    this->counter = length = order = nextMessage = 0;
    this->activated = true;
    this->SetMessage(label);
  }
  ~Messagebox() {
    this->text = nullptr;
    delete this->text;
  }

  void Draw(Graphics^ g) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    //Select the image and the position
    g->DrawImage(bmpManager->GetImage("assets\\sprites\\misc\\messagebox.png"), Point(101,230));
  }

  //Set the message and len
  void SetMessage(Label^ label) {
    this->text = label->Text;
    this->length = this->text->Length;
    label->Text = "";
  }

  //Print letter by letter
  void PrintLetter(Label^ label, Timer^ timer) {
    // Time to change to the other message
    short timeNextMessage = 20;

    //Set Color and Interval to Assasins and Corrupts
    if (this->order == 5) {
      timer->Interval = 50;
      label->ForeColor = Color::LightGreen;
    }
    else if (this->order % 2) {
      timer->Interval = 50;
      label->ForeColor = Color::LightSteelBlue;
    }
    else {

      if (this->order != 4)
        timer->Interval = 80;
      //At the last message of the assasin, go faster
      else
        timer->Interval = 20;

      label->ForeColor = Color::Crimson;
    }

    // Counter + 1
    this->counter++;
    //If the counter is greater to the length of the message
    if (this->counter > this->length) {
      timer->Interval = 100;
      // NextMessage +1
      this->nextMessage++;
      //If NextMessage reach 30
      if (this->nextMessage == timeNextMessage) {

        //If the order is 6, the messagebox will be deleted
        if (this->order == 5) {
          this->activated = false;
          label->Visible = false;
          timer->Stop();
          return;
        }

        //NextMessage return to 0
        this->nextMessage -= timeNextMessage;
        //Counter returns to 0
        this->counter -= (this->length + timeNextMessage);
        //Set the new message
        switch (this->order)
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

        this->order++;
        this->SetMessage(label);
      }
    }
    else {
      //Print a letter of the sring in the label
      label->Text = this->text->Substring(0, this->counter);
    }
  }

  bool GetActivated() {
    return this->activated;
  }
};
#endif // !_MESSAGEBOX_H_
