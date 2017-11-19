///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedMesh_hpp
#define YTE_Graphics_Generics_InstantiatedMesh_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  class InstantiatedModel : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedModel);

    virtual ~InstantiatedModel()
    {
      
    }

    virtual void UpdateUBOModel(UBOModel &aUBO)
    {
      YTEUnusedArgument(aUBO);
    }



    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    virtual Mesh* GetMesh()
    {
      return mMesh;
    }



  protected:
    Mesh *mMesh;
    UBOModel mUBOModelData;
  };
}

#endif
