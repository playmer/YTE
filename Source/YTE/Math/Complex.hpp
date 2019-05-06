#pragma once
#ifndef YTE_Math_Complex_hpp
#define YTE_Math_Complex_hpp


// --------------------------
// Declarations

namespace YTE
{
  struct complex
  {
    // default ctor
    complex() : mReal(0.0f), mImaginary(0.0f)
    {

    }


    // ------------------------------------
    // ctor
    complex(const float aReal, const float aImaginary) : mReal(aReal), mImaginary(aImaginary)
    {

    }


    // ------------------------------------
    // assignment operator
    complex& operator=(const complex& rhs)
    {
      mReal = rhs.mReal;
      mImaginary = rhs.mImaginary;
      return *this;
    }


    // ------------------------------------
    // add to me op
    complex& operator+=(const complex& rhs)
    {
      mReal += rhs.mReal;
      mImaginary += rhs.mImaginary;
      return *this;
    }


    // ------------------------------------
    // subtract from me opp
    complex& operator-=(const complex& rhs)
    {
      mReal -= rhs.mReal;
      mImaginary -= rhs.mImaginary;
      return *this;
    }


    // ------------------------------------
    // dot product me
    complex& operator*=(const complex& rhs)
    {
      mReal = (mReal * rhs.mReal) - (mImaginary * rhs.mImaginary);
      mImaginary = (mReal * rhs.mImaginary) + (mImaginary * rhs.mReal);
      return *this;
    }


    // ------------------------------------
    // conjugate returns the { real, -imaginary }
    complex conjugate()
    {
      return complex(mReal, -mImaginary);
    }


    // ------------------------------------
    // mulitply complex numbers
    complex operator*(const complex& rhs) const
    {
      float aa_sub_bb = (mReal * rhs.mReal) - (mImaginary * rhs.mImaginary);
      float ab_pls_ba = (mReal * rhs.mImaginary) + (mImaginary * rhs.mReal);

      return complex(aa_sub_bb, ab_pls_ba);
    }


    // ------------------------------------
    // scale complex number by a real number
    complex operator*(const float aScalar) const
    {
      return complex(mReal * aScalar, mImaginary * aScalar);
    }


    // ------------------------------------
    // scale complex number by a real number
    complex operator*=(const float aScalar)
    {
      mReal *= aScalar;
      mImaginary *= aScalar;
      return *this;
    }


    // ------------------------------------
    // addition of complex numbers
    complex operator+(const complex& rhs) const
    {
      return complex(mReal + rhs.mReal, mImaginary + rhs.mImaginary);
    }


    // ------------------------------------
    // subtraction of complex numbers
    complex operator-(const complex& rhs) const
    {
      return complex(mReal - rhs.mReal, mImaginary - rhs.mImaginary);
    }


    // ------------------------------------
    // equivlency operator
    bool operator==(const complex& rhs) const
    {
      if (mReal == rhs.mReal && mImaginary == rhs.mImaginary)
      {
        return true;
      }
      return false;
    }


    // ------------------------------------
    float mReal;
    float mImaginary;
  };
}


#endif