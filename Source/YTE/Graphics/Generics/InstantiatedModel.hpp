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

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  class InstantiatedModel : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedModel);

    InstantiatedModel();

    virtual ~InstantiatedModel()
    {
      
    }

    virtual void SetDefaultAnimationOffset()
    {
      
    }

    virtual bool GetInstanced()
    {
      return false;
    }

    virtual void UpdateUBOAnimation(UBOAnimation *aUBO)
    {
      YTEUnusedArgument(aUBO);
    }


    virtual void SetInstanced(bool aInstanced)
    {
      YTEUnusedArgument(aInstanced);
    }

    // Used to update with the previous data. This is useful for switching
    // between instancing or no instancing.
    virtual void UpdateUBOModel()
    {
    }

    virtual void UpdateUBOModel(UBOModel &aUBO)
    {
      YTEUnusedArgument(aUBO);
    }

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    Mesh* GetMesh()
    {
      return mMesh;
    }

  protected:
    Mesh *mMesh;
    UBOModel mUBOModelData;
    UBOAnimation *mUBOAnimationData;
  };
}

#endif
