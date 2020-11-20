#pragma once

#ifndef _MESSAGEBOX_H_
#define _MESSAGEBOX_H_

#include "BitmapManager.h"

ref class Messagebox {
  bool activated;
  unsigned int counter, length, order, nextMessage;
  String^ text;
public:
  Messagebox() {
    this->counter = length = order = nextMessage = 0;
  }
  ~Messagebox() {
    delete this->text;
  }
  void Draw(Graphics^ g) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    //Select the image and the position
    g->DrawImage(bmpManager->GetImage("assets\\sprites\\misc\\messagebox.png"), Point(101,230));
  }

  void SetActivated(bool value) {
    this->activated = value;
  }

  void SetCounter(int value) {
    this->counter += value;
  }

  void SetNextMessage(int value) {
    this->nextMessage += value;
  }

  void SetOrder(int value) {
    this->order += value;
  }

  void SetLength(int value) {
    this->length = value;
  }

  void SetText(String^ txt) {
    this->text = txt;
  }

  bool GetActivated() {
    return this->activated;
  }

  unsigned int GetCounter() {
    return this->counter;
  }

  unsigned int GetNextMessage() {
    return this->nextMessage;
  }

  unsigned int GetLength() {
    return this->length;
  }

  unsigned int GetOrder() {
    return this->order;
  }

  String^ GetText() {
    return this->text;
  }

};
#endif // !_MESSAGEBOX_H_
