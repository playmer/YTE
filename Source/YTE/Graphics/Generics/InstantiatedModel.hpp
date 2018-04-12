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
  enum class ShaderType
  {
    Triangles,
    Lines,
    Curves,
    Wireframe,
    ShaderNoCull,
    AlphaBlendShader,
    AdditiveBlendShader,
  };

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


    virtual void UpdateUBOSubmeshMaterial(UBOMaterial *aUBO, size_t aIndex)
    {
      YTEUnusedArgument(aUBO);
      YTEUnusedArgument(aIndex);
    }

    virtual void UpdateUBOMaterial(UBOMaterial *aUBO)
    {
      YTEUnusedArgument(aUBO);
    }

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    UBOModel& GetUBOModelData()
    {
      return mUBOModelData;
    }

    
    UBOMaterial GetUBOMaterialData()
    {
      return mUBOModelMaterialData;
    }

    Mesh* GetMesh()
    {
      return mMesh;
    }


    void SetVisibility(bool aVisibility)
    {
      mVisibility = aVisibility;
    }

    bool GetVisibility()
    {
      return mVisibility;
    }

    ShaderType mType = ShaderType::Triangles;
    float mLineWidth = 1.0f;

  protected:
    Mesh *mMesh;
    UBOModel mUBOModelData;
    UBOAnimation *mUBOAnimationData;
    UBOMaterial mUBOModelMaterialData;
    bool mVisibility = true;
  };
}

#endif
