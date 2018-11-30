///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedMesh_hpp
#define YTE_Graphics_Generics_InstantiatedMesh_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vertex.hpp"
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

    YTE_Shared InstantiatedModel();

    virtual ~InstantiatedModel()
    {
      
    }

    virtual void SetDefaultAnimationOffset()
    {
      
    }

    virtual void UpdateMesh(size_t aIndex, std::vector<Vertex>& aVertices)
    {
      UnusedArguments(aIndex, aVertices);
    }

    virtual void UpdateMesh(size_t aIndex, 
                            std::vector<Vertex>& aVertices, 
                            std::vector<u32>& aIndices)
    {
      UnusedArguments(aIndex, aVertices, aIndices);
    }

    virtual void UpdateUBOAnimation(UBOs::Animation *aUBO)
    {
      UnusedArguments(aUBO);
    }

    // Used to update with the previous data. This is useful for switching
    // between instancing or no instancing.
    virtual void UpdateUBOModel()
    {
    }

    virtual void UpdateUBOModel(UBOs::Model &aUBO)
    {
      UnusedArguments(aUBO);
    }


    virtual void UpdateUBOSubmeshMaterial(UBOs::Material *aUBO, size_t aIndex)
    {
      UnusedArguments(aUBO, aIndex);
    }

    virtual void UpdateUBOMaterial(UBOs::Material *aUBO)
    {
      UnusedArguments(aUBO);
    }

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    UBOs::Model& GetUBOModelData()
    {
      return mUBOModelData;
    }

    
    UBOs::Material GetUBOMaterialData()
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
    UBOs::Model mUBOModelData;
    UBOs::Animation *mUBOAnimationData;
    UBOs::Material mUBOModelMaterialData;
    bool mVisibility = true;
  };
}

#endif
