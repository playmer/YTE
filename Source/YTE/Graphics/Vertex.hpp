//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Graphics_Vertex_hpp
#define YTE_Graphics_Vertex_hpp

#include <vector>

#include "glm/glm.hpp"

#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  struct Vertex
  {
    glm::vec3 mPosition;
    glm::vec3 mTextureCoordinates;
    glm::vec3 mNormal;
    glm::vec4 mColor;
    glm::vec3 mTangent;
    glm::vec3 mBinormal;
    glm::vec3 mBitangent;
    glm::vec3 mBoneWeights;
    glm::vec2 mBoneWeights2;
    glm::ivec3 mBoneIDs;
    glm::ivec2 mBoneIDs2;

    Vertex(glm::vec3 &aPosition,
           glm::vec3 &aTextureCoordinates,
           glm::vec3 &aNormal,
           glm::vec4 &aColor,
           glm::vec3 &aTangent,
           glm::vec3 &aBinormal,
           glm::vec3 &aBitangent,
           glm::vec3 &aBoneWeights,
           glm::vec2 &aBoneWeights2,
           glm::ivec3 &aBoneIDs,
           glm::ivec2 &aBoneIDs2)
      : mPosition(aPosition)
      , mTextureCoordinates(aTextureCoordinates)
      , mNormal(aNormal)
      , mColor(aColor)
      , mTangent(aTangent)
      , mBinormal(aBinormal)
      , mBitangent(aBitangent)
      , mBoneWeights(aBoneWeights)
      , mBoneWeights2(aBoneWeights2)
      , mBoneIDs(aBoneIDs)
      , mBoneIDs2(aBoneIDs2)
    {

    }

    Vertex()
    {
      
    }
  };

  struct Instance
  { 
    glm::vec4 mMatrix0;
    glm::vec4 mMatrix1;
    glm::vec4 mMatrix2;
    glm::vec4 mMatrix3;

    Instance(UBOs::Model aModel)
      : mMatrix0(aModel.mModelMatrix[0])
      , mMatrix1(aModel.mModelMatrix[1])
      , mMatrix2(aModel.mModelMatrix[2])
      , mMatrix3(aModel.mModelMatrix[3])
    {

    }
    
    Instance(glm::vec4 aMatrix0,
             glm::vec4 aMatrix1,
             glm::vec4 aMatrix2,
             glm::vec4 aMatrix3)
      : mMatrix0(aMatrix0)
      , mMatrix1(aMatrix1)
      , mMatrix2(aMatrix2)
      , mMatrix3(aMatrix3)
    {

    }

    Instance()
    {

    }
  };
}

#endif
