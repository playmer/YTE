//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Vertex_hpp
#define YTE_Vertex_hpp

#include <vector>
#include "glm/glm.hpp"

namespace YTE
{
  struct Vertex
  {
    glm::vec3 mPosition;
    glm::vec3 mTextureCoordinates;
    glm::vec3 mNormal;
    glm::vec3 mColor;
    glm::vec3 mTangent;
    glm::vec3 mBinormal;
    glm::vec3 mBitangent;

    Vertex(glm::vec3 &aPosition,
           glm::vec3 &aTextureCoordinates,
           glm::vec3 &aNormal,
           glm::vec3 &aColor,
           glm::vec3 &aTangent,
           glm::vec3 &aBinormal,
           glm::vec3 &aBitangent)
      : mPosition(aPosition)
      , mTextureCoordinates(aTextureCoordinates)
      , mNormal(aNormal)
      , mColor(aColor)
      , mTangent(aTangent)
      , mBinormal(aBinormal)
      , mBitangent(aBitangent)
    {

    }



    Vertex()
    {
      
    }
  };
}

#endif
