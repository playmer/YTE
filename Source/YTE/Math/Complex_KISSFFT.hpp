#pragma once
#ifndef YTE_Math_Complex_KISSFFT_hpp
#define YTE_Math_Complex_KISSFFT_hpp

// cannot include namespace to make complient with kiss fft: https://sourceforge.net/projects/kissfft/
#include <kiss_fft.h>     // kfft complex
#include "YTE/Math/Complex.hpp"    // yte complex
#include <vector>         // vector in class

// kiss_fft states that without this defined, float is used, with defined, either int16 or int 32 is used
#ifdef FIXED_POINT
#error "ERROR: YTE/Math/Complex_KISSFFT.hpp detecting that kiss_fft has FIXED_POINT defined, complex_kfft requires default to be used, please undefine FIXED_POINT from project"
#endif


// --------------------------
// Declarations

/**********************/
// This class allows the combination of the kiss_fft complex and the YTE complex
// This also wraps it into a vector since water simulation needs a vector and it makes
// the reinterpret_cast work
/**********************/
class complex_kfft
{
public:
  complex_kfft()
  {

  }


  // ------------------------------------
  ~complex_kfft()
  {
  }


  // ------------------------------------
  // returns the vector of YTE complex
  std::vector<YTE::complex>& GetVector()
  {
    return mComplexes;
  }


  // ------------------------------------
  // gets the actual data elements form the vector as a c-style array
  YTE::complex* GetComplexArray()
  {
    return mComplexes.data();
  }


  // ------------------------------------
  // returns a c-style array of YTE complex reinterpret_cast to kiss_fft complex
  kiss_fft_cpx* GetKFFTArray()
  {
    return reinterpret_cast<kiss_fft_cpx*>(mComplexes.data());
  }


  // ------------------------------------
  // since class wraps around a vector, this is used to have a more simple accessing method
  YTE::complex& operator[](int index)
  {
    return mComplexes[index];
  }


private:
  std::vector<YTE::complex> mComplexes;
};



#endif