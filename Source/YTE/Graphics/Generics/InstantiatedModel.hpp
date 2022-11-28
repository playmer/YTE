#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedMesh_hpp
#define YTE_Graphics_Generics_InstantiatedMesh_hpp

#include <map>
#include <variant>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/GPUBuffer.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Vertex.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"

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

    using OwnedBuffer  = std::unique_ptr<GPUBufferBase>;
    using ObservedBuffer = GPUBufferBase *;
    using BufferRef = std::variant<OwnedBuffer, ObservedBuffer>;

    YTE_Shared InstantiatedModel(Renderer* aRenderer);

    void Create();

    virtual ~InstantiatedModel()
    {
      
    }

    void SetDefaultAnimationOffset();

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

    template <typename tType>
    GPUBufferRef<tType> GetBufferRef()
    {
      auto it = mTypeToIndex.find(TypeId<tType>());

      if (it != mTypeToIndex.end())
      {
        if (auto ownedBuffer = std::get_if<OwnedBuffer>(&mBuffers[it->second]))
        {
          return GPUBufferRef<tType>{ **ownedBuffer };
        }
        else if (auto observedBuffer = std::get_if<ObservedBuffer>(&mBuffers[it->second]))
        {
          return GPUBufferRef<tType>{ **observedBuffer };
        }
      }

      return { nullptr };
    }

    std::vector<std::variant<OwnedBuffer, ObservedBuffer>>& GetBuffers()
    {
      return mBuffers;
    }

    // Does not take ownership of the buffer
    template <typename tType>
    void AddBuffer(GPUBufferBase* aBufferRef)
    {
      mBuffers.emplace_back(aBufferRef);
      mTypeToIndex.emplace(TypeId<tType>(), mBuffers.size());
    }

    // Does not takes ownership of the buffer
    template <typename tType>
    void AddBuffer(GPUBuffer<tType>* aBufferRef)
    {
      AddBuffer<tType>(&aBufferRef->GetBase());
    }

    // Takes ownership of the buffer
    template <typename tType>
    void AddBuffer(GPUBuffer<tType>& aBufferRef)
    {
      mBuffers.emplace_back(aBufferRef.Steal());
      mTypeToIndex.emplace(TypeId<tType>(), mBuffers.size());
    }

  protected:
    std::vector<BufferRef> mBuffers;
    std::multimap<Type*, size_t> mTypeToIndex;

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
