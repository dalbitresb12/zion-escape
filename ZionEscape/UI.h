#pragma once

#ifndef _UI_H_
#define _UI_H_

#include "BitmapManager.h"
#include "Enums.h"
#include "MathUtils.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace MathUtils::Mathf;

ref class UI {
  static Font^ titleFont = gcnew Font("Impact", 74);
  static Font^ optionsFont = gcnew Font("Impact", 40);
  static Dictionary<String^, Rectangle>^ textRectangles = gcnew Dictionary<String^, Rectangle>;

  UI() {}
  ~UI() {}

public:
  static void DrawMenu(Graphics^ world, Size clientSize, Point mouseLoc) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ background = bmpManager->GetImage("assets\\sprites\\misc\\menu-bg.png");

    if (textRectangles->Count == 0) {
      PreprocessText(world, clientSize);
    }

    world->DrawImage(background, Point(0, 0));
    CreateMenu(world, clientSize, mouseLoc);
  }

  static void DrawCredits(Graphics^ world, Point mouseLoc) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ background = bmpManager->GetImage("assets\\sprites\\misc\\menu-bg.png");
    Bitmap^ credits = bmpManager->GetImage("assets\\sprites\\misc\\credits.png");
    world->DrawImage(background, Point(0, 0));
    world->DrawImage(credits, Point(0, 0));

    Rectangle backRectangle = GetTextRectangle("Volver");
    Brush^ backBrush = IsHovering(backRectangle, mouseLoc) ? Brushes::SlateGray : Brushes::White;
    world->DrawString("Volver", optionsFont, backBrush, backRectangle.Location);
  }

  static void DrawPause(Graphics^ world, Point mouseLoc) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ background = bmpManager->GetImage("assets\\sprites\\misc\\menu-bg.png");
    world->DrawImage(background, Point(0, 0));

    Rectangle pauseRectangle = GetTextRectangle("Pausa");
    world->DrawString("Pausa", titleFont, Brushes::White, pauseRectangle.Location);

    Rectangle exitRectangle = GetTextRectangle("Salir");
    Brush^ exitBrush = IsHovering(exitRectangle, mouseLoc) ? Brushes::SlateGray : Brushes::White;
    world->DrawString("Salir", optionsFont, exitBrush, exitRectangle.Location);

    Rectangle backRectangle = GetTextRectangle("Volver");
    Brush^ backBrush = IsHovering(backRectangle, mouseLoc) ? Brushes::SlateGray : Brushes::White;
    world->DrawString("Volver", optionsFont, backBrush, backRectangle.Location);
  }

  static UserInterface ClickEvent(Point mouseLoc, UserInterface currentUI) {
    for each (KeyValuePair<String^, Rectangle> element in textRectangles) {
      if (element.Key == "Zion Escape" || element.Key == "Pausa")
        continue;

      Rectangle rect = element.Value;

      if (IsHovering(rect, mouseLoc)) {
        

        if (currentUI == UserInterface::Credits) {
          if (element.Key == "Volver")
            return UserInterface::MainMenu;
        }

        if (currentUI == UserInterface::Pause) {
          if (element.Key == "Volver")
            return UserInterface::InGame;
          else if (element.Key == "Salir")
            Application::Exit();
        }

        if (currentUI == UserInterface::MainMenu) {
          if (element.Key == "Iniciar")
            return UserInterface::InGame;
          else if (element.Key == "Salir")
            Application::Exit();
          else if (element.Key == "Creditos")
            return UserInterface::Credits;
        }
      }
    }
    return currentUI;
  }

  static void DrawHearts(Graphics^ world, float healthPoints) {
    BitmapManager^ bmpManager = BitmapManager::GetInstance();
    Bitmap^ heartImage = bmpManager->GetImage("assets\\sprites\\misc\\heart.png");
    Rectangle drawingArea = Rectangle(Point(20, 15), Size(30, 30));

    // Draw all the healthPoints of the Player
    for (int i = 0; i < Math::Round(healthPoints); i++) {
      world->DrawImage(heartImage, drawingArea);
      // Add the X position of where the next heart will be drawn
      drawingArea.X += 33;
    }
  }

private:
  static void CreateMenu(Graphics^ world, Size clientSize, Point mouseLoc) {
    if (textRectangles->Count == 0) {
      PreprocessText(world, clientSize);
    }

    Brush^ textBrush = Brushes::White;

    Rectangle titleRectangle = GetTextRectangle("Zion Escape");
    world->DrawString("Zion Escape", titleFont, textBrush, titleRectangle.Location);
    
    Rectangle startRectangle = GetTextRectangle("Iniciar");
    if (IsHovering(startRectangle, mouseLoc))
      textBrush = Brushes::SlateGray;
    world->DrawString("Iniciar", optionsFont, textBrush, startRectangle.Location);
    textBrush = Brushes::White;

    Rectangle creditsRectangle = GetTextRectangle("Creditos");
    if (IsHovering(creditsRectangle, mouseLoc))
      textBrush = Brushes::SlateGray;
    world->DrawString("Creditos", optionsFont, textBrush, creditsRectangle.Location);
    textBrush = Brushes::White;

    Rectangle exitRectangle = GetTextRectangle("Salir");
    if (IsHovering(exitRectangle, mouseLoc))
      textBrush = Brushes::SlateGray;
    world->DrawString("Salir", optionsFont, textBrush, exitRectangle.Location);
    textBrush = Brushes::White;
  }

  static void PreprocessText(Graphics^ world, Size clientSize) {
    if (!textRectangles->ContainsKey("Zion Escape")) {
      CreateText("Zion Escape", world, titleFont, clientSize, Size(0, -200));
    }
    if (!textRectangles->ContainsKey("Iniciar")) {
      CreateText("Iniciar", world, optionsFont, clientSize, Size(0, -50));
    }
    if (!textRectangles->ContainsKey("Creditos")) {
      CreateText("Creditos", world, optionsFont, clientSize, Size(0, 50));
    }
    if (!textRectangles->ContainsKey("Salir")) {
      CreateText("Salir", world, optionsFont, clientSize, Size(0, 150));
    }
    if (!textRectangles->ContainsKey("Pausa")) {
      CreateText("Pausa", world, titleFont, clientSize, Size(0, 0));
    }
    if (!textRectangles->ContainsKey("Volver")) {
      CreateText("Volver", world, optionsFont, clientSize, Size(-350, -250));
    }
  }

  static Rectangle GetTextRectangle(String^ key) {
    Rectangle rect;
    if (textRectangles->ContainsKey(key))
      textRectangles->TryGetValue(key, rect);
    return rect;
  }

  static void CreateText(String^ text, Graphics^ world, Font^ font, Size clientSize, Size offset) {
    Size size = Size::Round(world->MeasureString(text, font));
    Point position = GetPositionFromBounds(clientSize, size, offset);
    textRectangles->Add(text, Rectangle(position, size));
  }

  static bool IsHovering(Rectangle rect, Point mouseLoc) {
    return rect.Contains(mouseLoc);
  }

  static Point GetPositionFromBounds(Size clientSize, Size bounds, Size offset) {
    int x = RoundToInt((float)clientSize.Width / 2 - bounds.Width / 2 + offset.Width);
    int y = RoundToInt((float)clientSize.Height / 2 - bounds.Height / 2 + offset.Height);
    return Point(x, y);
  }

  static RectangleF GetTextBounds(String^ text, Font^ font) {
    return GetTextBounds(text, font, StringFormat::GenericDefault);
  }

  static RectangleF GetTextBounds(String^ text, Font^ font, StringFormat^ stringFormat) {
    GraphicsPath^ path = gcnew GraphicsPath();
    path->AddString(text, font->FontFamily, (int)font->Style, font->Size, Point(0, 0), stringFormat);
    return path->GetBounds();
  }
};

#endif // !_UI_H_
