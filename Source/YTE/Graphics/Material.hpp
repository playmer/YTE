#pragma once

#ifndef YTE_Graphics_Material_hpp
#define YTE_Graphcis_Material_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class Material;

  class MaterialRepresentation : public EventHandler
  {
  public:
    YTEDeclareType(MaterialRepresentation);

    MaterialRepresentation(Material *aMaterialComponent, RSValue *aProperties = nullptr)
      : mMaterialComponent(aMaterialComponent)
    {
      DeserializeByType(aProperties, this, GetStaticType());
    }

    MaterialRepresentation(UBOs::Material aMaterial, 
                           Material *aMaterialComponent, 
                           size_t aIndex, 
                           Submesh *aSubmesh)
      : mMaterialComponent(aMaterialComponent)
      , mMaterial(aMaterial)
      , mIndex(aIndex)
      , mSubmesh(aSubmesh)
    {

    }

    void UpdateUBO();

    void SetDiffuse(glm::vec3 aDiffuse)
    {
      mMaterial.mDiffuse = glm::vec4{ aDiffuse, 1.0f };
      UpdateUBO();
    }

    void SetAmbient(glm::vec3 aAmbient)
    {
      mMaterial.mAmbient = glm::vec4{ aAmbient, 1.0f };
      UpdateUBO();
    }

    void SetSpecular(glm::vec4 aSpecular)
    {
      mMaterial.mSpecular = aSpecular;
      UpdateUBO();
    }

    void SetEmissive(glm::vec3 aEmissive)
    {
      mMaterial.mEmissive = glm::vec4{ aEmissive, 1.0f };
      UpdateUBO();
    }

    void SetTransparent(glm::vec4 aTransparent)
    {
      mMaterial.mTransparent = aTransparent;
      UpdateUBO();
    }

    void SetReflective(glm::vec4 aReflective)
    {
      mMaterial.mReflective = aReflective;
      UpdateUBO();
    }

    void SetOpacity(float aOpacity)
    {
      mMaterial.mOpacity = aOpacity;
      UpdateUBO();
    }

    void SetShininess(float aShininess)
    {
      mMaterial.mShininess = aShininess;
      UpdateUBO();
    }

    void SetShininessStrength(float aShininessStrength)
    {
      mMaterial.mShininessStrength = aShininessStrength;
      UpdateUBO();
    }

    void SetReflectivity(float aReflectivity)
    {
      mMaterial.mReflectivity = aReflectivity;
      UpdateUBO();
    }

    void SetReflectiveIndex(float aReflectiveIndex)
    {
      mMaterial.mReflectiveIndex = aReflectiveIndex;
      UpdateUBO();
    }

    void SetBumpScaling(float aBumpScaling)
    {
      mMaterial.mBumpScaling = aBumpScaling;
      UpdateUBO();
    }

    void SetFlag(UBOs::MaterialFlags aFlag, bool aValue)
    {
      // Set flag
      if (aValue)
      {
        mMaterial.mFlags |= (1u << ((u32)aFlag / 2));
      }
      // Clear Flag
      else
      {
        mMaterial.mFlags &= ~(1u << ((u32)aFlag / 2));
      }

      UpdateUBO();
    }

    void SetUseNormalTexture(bool aUseNormalTexture)
    {
      mMaterial.mUsesNormalTexture = aUseNormalTexture ? 1 : 0;
      UpdateUBO();
    }

    glm::vec3 GetDiffuse()  { return glm::vec3{mMaterial.mDiffuse.x,  mMaterial.mDiffuse.y,  mMaterial.mDiffuse.z  }; }
    glm::vec3 GetAmbient()  { return glm::vec3{mMaterial.mAmbient.x,  mMaterial.mAmbient.y,  mMaterial.mAmbient.z  }; }
    glm::vec4 GetSpecular() { return mMaterial.mSpecular; }
    glm::vec3 GetEmissive() { return glm::vec3{ mMaterial.mEmissive.x, mMaterial.mEmissive.y, mMaterial.mEmissive.z }; }
    glm::vec4 GetTransparent() { return mMaterial.mTransparent; }
    glm::vec4 GetReflective() { return mMaterial.mReflective; }
    float     GetOpacity() { return mMaterial.mOpacity; }
    float     GetShininess() { return mMaterial.mShininess; }
    float     GetShininessStrength() { return mMaterial.mShininessStrength; }
    float     GetReflectivity() { return mMaterial.mReflectivity; }
    float     GetReflectiveIndex() { return mMaterial.mReflectiveIndex; }
    float     GetBumpScaling() { return mMaterial.mBumpScaling; }
    u32       GetFlags() { return mMaterial.mFlags; }
    bool      GetUseNormalTexture() { return mMaterial.mUsesNormalTexture ? true : false; }


    Submesh* GetSubmesh() { return mSubmesh; }
    void SetSubmesh(Submesh *aSubmesh) { mSubmesh = aSubmesh; }

    size_t GetIndex() { return mIndex; }
    void SetIndex(size_t aIndex) { mIndex = aIndex; }
  private:
    Material *mMaterialComponent{ nullptr };
    UBOs::Material mMaterial;
    size_t mIndex{ 0 };
    std::string mDiffuseTexture;
    std::string mSpecularTexture;
    std::string mNormalTexture;

    Submesh *mSubmesh{ nullptr };
  };


  class Material : public Component
  {
  public:
    YTEDeclareType(Material);
    Material(Material&) = delete;

    Material(Composition *aOwner, Space *aSpace);
    ~Material() override;

    void Initialize() override;
    void Create(ModelChanged *aEvent);
    static std::vector<std::pair<YTE::Object*, std::string>> SubmeshMaterialLister(YTE::Object *aSelf);
    static RSValue SubmeshMaterialSerializer(RSAllocator &aAllocator, Object *aOwner);
    static void SubmeshMaterialDeserializer(RSValue &aValue, Object *aOwner);



    static std::pair<YTE::Object*, std::string> ModelMaterialGetObject(YTE::Object *aSelf);
    static RSValue ModelMaterialSerializer(RSAllocator &aAllocator, Object *aOwner);
    static void ModelMaterialDeserializer(RSValue &aValue, Object *aOwner);

  private:
    //Renderer *mRenderer;
    //Window *mWindow;
    Model *mModel;
    FFT_WaterSimulation *mFFTWater;
    std::string mName;

    MaterialRepresentation mModelMaterial;
    std::vector<std::unique_ptr<MaterialRepresentation>> mSubmeshMaterials;
  };
}

#endif
