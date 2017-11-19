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

  struct SpriteVertex
  {
    glm::vec3 mPosition;
    glm::vec2 mTextureCoordinates;
    glm::vec3 mNormal;

    SpriteVertex(glm::vec3 &aPosition,
                 glm::vec2 &aTextureCoordinates,
                 glm::vec3 &aNormal)
      : mPosition(aPosition)
      , mTextureCoordinates(aTextureCoordinates)
      , mNormal(aNormal)
    {

    }

    SpriteVertex()
    {

    }
  };

  struct SpriteInstance
  {
    u32 mTextureId;
    glm::vec4 mMatrix1;
    glm::vec4 mMatrix2;
    glm::vec4 mMatrix3;
    glm::vec4 mMatrix4;

    
    SpriteInstance(u32 aTextureId,
                   glm::vec4 aMatrix1,
                   glm::vec4 aMatrix2,
                   glm::vec4 aMatrix3,
                   glm::vec4 aMatrix4)
      : mTextureId(aTextureId)
      , mMatrix1(aMatrix1)
      , mMatrix2(aMatrix2)
      , mMatrix3(aMatrix3)
      , mMatrix4(aMatrix4)
    {

    }

    SpriteInstance()
    {

    }
  };
}

#endif
