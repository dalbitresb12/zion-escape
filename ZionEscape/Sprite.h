#pragma once

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Enums.h"

using namespace System;
using namespace System::Drawing;

ref class Sprite abstract {
  Bitmap^ image;
  Size size;
  short col, row, nCols, nRows;
  bool animatable;

protected:
  Rectangle drawingArea;

public:
  Sprite(bool animatable) {
    this->image = nullptr;
    this->nCols = 0;
    this->nRows = 0;
    this->col = 0;
    this->row = 0;
    this->animatable = animatable;
  }

  Sprite(Bitmap^ image, short nCols, short nRows, bool animatable) {
    this->SetImage(image, nCols, nRows);
    this->col = 0;
    this->row = 0;
    this->animatable = animatable;
  }
  ~Sprite() {}

  void SetImage(Bitmap^ image, short nCols, short nRows) {
    this->image = image;
    this->size = Size(image->Width / nCols, image->Height / nRows);
    this->drawingArea.Size = this->size;
    this->col = 0;
    this->row = 0;
    this->nCols = nCols;
    this->nRows = nRows;
  }

  Size GetSize() {
    return drawingArea.Size;
  }

  void SetPosition(int x, int y) {
    SetPosition(Point(x, y));
  }

  void SetPosition(Point point) {
    this->drawingArea.Location = point;
  }

  Point GetPosition() {
    return Point(drawingArea.X + size.Width / 2, drawingArea.Y + size.Height / 2);
  }

  void StartAnimation() {
    SetAnimatable(true);
  }

  virtual void StopAnimation() {
    SetAnimatable(false);
    col = 0;
  }

  void SetAnimatable(bool animatable) {
    this->animatable = animatable;
  }

  bool IsAnimatable() {
    return animatable;
  }

  short GetCol() {
    return col;
  }

  void SetCol(short index) {
    this->col = index;
  }

  void ShiftCol() {
    if (this->animatable)
      this->col = (this->col + 1) % this->nCols;
  }

  short GetRow() {
    return row;
  }

  void SetRow(short index) {
    this->row = index;
  }

  virtual void SetSpriteDirection(Direction direction) abstract;

  void Draw(Graphics^ world) {
    if (image == nullptr) return;

    world->DrawImage(image, this->drawingArea, this->GetCropArea(), GraphicsUnit::Pixel);
  }

  Rectangle GetCropArea() {
    Point pos = Point(col * size.Width, row * size.Height);
    return Rectangle(pos, size);
  }

  Rectangle GetDrawingArea() {
    return this->drawingArea;
  }
};

#endif // !_SPRITE_H_

