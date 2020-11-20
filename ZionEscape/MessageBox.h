#pragma once

#ifndef _MESSAGEBOX_H_
#define _MESSAGEBOX_H_

#include "BitmapManager.h"

ref class Messagebox {
  bool activated;
  BitmapManager^ bmpManager;
  int counter, len;
  String^ txt;
public:
  Messagebox() {
    this->bmpManager = BitmapManager::GetInstance();
    this->counter = len = 0;
  }
  ~Messagebox() {
    delete this->bmpManager;
    delete this->txt;
  }
  void Draw(Graphics^ g) {
    //Select the image and the position
    g->DrawImage(bmpManager->GetImage("assets\\sprites\\misc\\messagebox.png"), Point(101,230));
  }

  void SetActivated(bool value) {
    this->activated = value;
  }

  void SetCounter(int value) {
    this->counter += value;
  }

  void SetLen(int value) {
    this->len = value;
  }

  void SetTxt(String^ txt) {
    this->txt = txt;
  }

  bool GetActivated() {
    return this->activated;
  }

  int GetCounter() {
    return this->counter;
  }

  int GetLen() {
    return this->len;
  }

  String^ GetTxt() {
    return this->txt;
  }

};
#endif // !_MESSAGEBOX_H_
