#pragma once

#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

using namespace System;

namespace MathUtils {
  namespace Mathf {
    static float Clamp(float value, float min, float max) {
      if (value < min)
        return min;
      else if (value > max)
        return max;
      else
        return value;
    }

    static double Clamp(double value, double min, double max) {
      if (value < min)
        return min;
      else if (value > max)
        return max;
      else
        return value;
    }

    static float Clamp01(float value) {
      return Clamp(value, 0.f, 1.f);
    }

    static double Clamp01(double value) {
      return Clamp(value, 0.0, 1.0);
    }

    static int RoundToInt(float value) {
      return (int)Math::Round(value);
    }

    static int RoundToInt(double value) {
      return (int)Math::Round(value);
    }
  }
}

#endif // !_MATHUTILS_H_

