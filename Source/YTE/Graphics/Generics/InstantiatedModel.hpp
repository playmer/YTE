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

    YTE_Shared InstantiatedModel(Renderer* aRenderer);

    void Create();

    virtual ~InstantiatedModel()
    {
      
    }

    void SetDefaultAnimationOffset();

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

    void UpdateUBOModel(UBOs::Model const& aUBO)
    {
      mUBOModelData = aUBO;
      mModelUBO.Update(mUBOModelData);
    }

    void UpdateUBOAnimation(UBOs::Animation *aUBO)
    {
      mUBOAnimationData = aUBO;
      mAnimationUBO.Update(*mUBOAnimationData);
    }

    virtual void UpdateUBOSubmeshMaterial(UBOs::Material *aUBO, size_t aIndex)
    {
      auto& [buffer, ubo] = mSubmeshMaterialsUBO[aIndex];

      ubo = *aUBO;
      buffer.Update(ubo);
    }

    void UpdateUBOMaterial(UBOs::Material *aUBO)
    {
      mUBOModelMaterialData = *aUBO;
      mModelMaterialUBO.Update(mUBOModelMaterialData);
    }

    /////////////////////////////////
    // Getters / Setters
    /////////////////////////////////
    UBOs::Model const& GetUBOModelData()
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

    GPUBuffer<UBOs::Model>& GetModelUBOBuffer()
    {
      return mModelUBO;
    }

    GPUBuffer<UBOs::Animation>& GetAnimationUBOBuffer()
    {
      return mAnimationUBO;
    }

    GPUBuffer<UBOs::Material>& GetModelMaterialUBOBuffer()
    {
      return mModelMaterialUBO;
    }

  protected:
    Renderer* mRenderer;
    Mesh *mMesh;
    GPUBuffer<UBOs::Model> mModelUBO;
    GPUBuffer<UBOs::Animation> mAnimationUBO;
    GPUBuffer<UBOs::Material> mModelMaterialUBO;
    std::vector<std::pair<GPUBuffer<UBOs::Material>, UBOs::Material>> mSubmeshMaterialsUBO;
    UBOs::Model mUBOModelData;
    UBOs::Animation *mUBOAnimationData;
    UBOs::Material mUBOModelMaterialData;
    bool mVisibility = true;

    static UBOs::Animation cAnimation;
  };
}

#endif
