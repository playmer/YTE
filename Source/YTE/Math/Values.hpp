#pragma once
#ifndef YTE_Math_Values_hpp
#define YTE_Math_Values_hpp

// --------------------------
// Declarations

namespace YTE
{
  // pi constants
  const float pi = 3.14159265358979323846f;
  const float piDiv2 = pi / 2.0f;
  const float piDiv4 = piDiv2 / 2.0f;
  const float piMul2 = 2.0f * pi;
  const float piMul4 = 2.0f * piMul2;

  const double pi_d = 3.14159265358979323846;
  const double piDiv2_d = pi_d / 2.0;
  const double piDiv4_d = piDiv2_d / 2.0;
  const double piMul2_d = 2.0 * pi_d;
  const double piMul4_d = 2.0 * piMul2_d;

  // squares stuff
  template <typename T>
  T squared(const T& t)
  {
    return t * t;
  }

  // checks if a float is close to zero enough to be considered zero
  inline bool floatNotZero(const float aFloat)
  {
    // less than small positive
    if (aFloat < 0.00000001f)
    {
      // and less than small negative
      if (aFloat > -0.00000001f)
      {
        return true;  // basically zero
      }
    }

    return false; // not zero
  }
}



#endif