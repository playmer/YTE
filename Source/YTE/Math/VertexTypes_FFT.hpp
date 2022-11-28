#pragma once
#ifndef YTE_Math_VertexTypes_FFT_hpp
#define YTE_Math_VertexTypes_FFT_hpp

#include <glm/glm.hpp>  // glm
#include "YTE/Math/Complex.hpp"  // complex


// --------------------------
// Declarations


namespace YTE
{
  struct WaterVertex
  {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec3 mUV;
  };


  // ------------------------------------
  struct WaterComputationalVertex
  {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec3 mHTilde0;
    glm::vec3 mHTilde0mkConjugate;
    glm::vec3 mOriginalPosition;
    glm::vec2 mTextureCoordinates;
  };


  // ------------------------------------
  struct WaterComplexNormalVertex
  {
    complex mWaveHeight;
    glm::vec2 mDisplacement;
    glm::vec3 mNormal;
  };
}



#endif