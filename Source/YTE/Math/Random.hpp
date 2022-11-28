#pragma once
#ifndef YTE_Math_Random_hpp
#define YTE_Math_Random_hpp

#include "YTE/Math/Complex.hpp"
#include <math.h>
#include <cstdlib>
#include "YTE/Math/Values.hpp"


// --------------------------
// Declarations

namespace YTE
{
  // random float is returned
  inline float randf()
  {
    return static_cast<float>(rand());
  }


  // ------------------------------------
  // random float between the bounds is returned
  inline float randf(int aLowerBound, int aUpperBound)
  {
    float r = randf() / static_cast<float>(RAND_MAX);
    return (((aUpperBound - aLowerBound) * r) + aLowerBound);
  }


  // ------------------------------------
  // uniform random float
  inline float RandomUniformFloat()
  {
    return randf() / static_cast<float>(RAND_MAX);
  }


  // ------------------------------------
  // uniform random float between the the bounds
  inline float RandomUniformFloat(float aLowerBound, float aUpperBound)
  {
    return randf(static_cast<int>(aLowerBound), static_cast<int>(aUpperBound));
  }


  // ------------------------------------
  // guassian random complex number
  inline complex RandomGuassian()
  {
    /* guassian is normally:
    * y1 = sqrt(-2 * ln(x1)) * cos(2 * pi * x2);
    * y2 = sqrt(-2 * ln(x1)) * sin(2 * pi * x2);
    * where x1 and x2 are random variables
    * This forces things to be between 0 and 1 since its periodic from sin and cos
    *
    * Instead you can do the following:
    */

    float x1;
    float x2;
    float check;

    do
    {
      x1 = 2.0f * RandomUniformFloat() - 1.0f;  // * 2 - 1 normalizes to 0-1
      x2 = 2.0f * RandomUniformFloat() - 1.0f;  // these are our randoms
      check = (x1 * x1) + (x2 * x2);  // geometrically this will check if it is between 0-1
    } while (check >= 1.0f);

    // now we have a value 0-1, we can do the rest of the computations
    check = sqrt((-2.0f * log(check)) / check); // also normalizes it

                                                // return the augmented values with the check
    return complex(x1 * check, x2 * check);
  }
}


#endif