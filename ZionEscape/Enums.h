#pragma once

#ifndef _ENUMS_H_
#define _ENUMS_H_

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

public enum class EntityType {
  Player, Obstacle, Assassin, Corrupt, Ally
};

public enum class Direction {
  Up, Down, Left, Right
};

// If you add another background image change the
// maximum value in the random getter below.
public enum class BackgroundImage {
  Scene1, Scene2, Scene3, Scene4
};

public enum class UserInterface {
  MainMenu, Credits, Pause, InGame, None
};

namespace EnumUtilities {
  static BackgroundImage GetRandomBackground(Random^ rnd) {
    // Set this maximum value to the number of elements in the enumaration.
    return (BackgroundImage)rnd->Next(4);
  }

  static String^ GetPathFromBackground(BackgroundImage image) {
    // Add the new path to the scene here.
    switch (image) {
      case BackgroundImage::Scene1:
        return "assets\\sprites\\scenes\\scene_1.png";
      case BackgroundImage::Scene2:
        return "assets\\sprites\\scenes\\scene_2.png";
      case BackgroundImage::Scene3:
        return "assets\\sprites\\scenes\\scene_3.png";
      case BackgroundImage::Scene4:
        return "assets\\sprites\\scenes\\scene_4.png";
      default:
        return "assets\\sprites\\scenes\\scene_1.png";
    }
  }

  static Direction GetInverseDirection(Direction dir) {
    switch (dir) {
      case Direction::Up:
        return Direction::Down;
      case Direction::Down:
        return Direction::Up;
      case Direction::Left:
        return Direction::Right;
      case Direction::Right:
        return Direction::Left;
      default:
        return Direction::Up;
    }
  }

  static Point GetPositionFromDirection(Point initialPos, Direction direction, int value) {
    Point finalPosition = Point(initialPos);
    if (direction == Direction::Up || direction == Direction::Down) {
      finalPosition.Y += direction == Direction::Up ? -value : value;
    } else {
      finalPosition.X += direction == Direction::Left ? -value : value;
    }
    return finalPosition;
  }
}

#endif // !_ENUMS_H_
