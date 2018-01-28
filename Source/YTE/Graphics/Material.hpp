//////////////////////////
// Author: Joshua T. Fisher
//////////////////////////

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
    
    MaterialRepresentation(Material *aMaterialComponent)
      : mMaterialComponent(aMaterialComponent)
    {

    }

    MaterialRepresentation(UBOMaterial aMaterial, 
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

    void SetDiffuse(glm::vec4 aDiffuse)
    {
      mMaterial.mDiffuse = aDiffuse;
      UpdateUBO();
    }

    void SetAmbient(glm::vec4 aAmbient)
    {
      mMaterial.mAmbient = aAmbient;
      UpdateUBO();
    }

    void SetSpecular(glm::vec4 aSpecular)
    {
      mMaterial.mSpecular = aSpecular;
      UpdateUBO();
    }

    void SetEmissive(glm::vec4 aEmissive)
    {
      mMaterial.mEmissive = aEmissive;
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

    void SetIsEditorObject(int aIsEditorObject)
    {
      mMaterial.mIsEditorObject = aIsEditorObject;
      UpdateUBO();
    }

    void SetPadding(float aPadding)
    {
      mMaterial.mPadding = aPadding;
      UpdateUBO();
    }

    glm::vec4 GetDiffuse() { return mMaterial.mDiffuse; }
    glm::vec4 GetAmbient() { return mMaterial.mAmbient; }
    glm::vec4 GetSpecular() { return mMaterial.mSpecular; }
    glm::vec4 GetEmissive() { return mMaterial.mEmissive; }
    glm::vec4 GetTransparent() { return mMaterial.mTransparent; }
    glm::vec4 GetReflective() { return mMaterial.mReflective; }
    float     GetOpacity() { return mMaterial.mOpacity; }
    float     GetShininess() { return mMaterial.mShininess; }
    float     GetShininessStrength() { return mMaterial.mShininessStrength; }
    float     GetReflectivity() { return mMaterial.mReflectivity; }
    float     GetReflectiveIndex() { return mMaterial.mReflectiveIndex; }
    float     GetBumpScaling() { return mMaterial.mBumpScaling; }
    int       GetIsEditorObject() { return mMaterial.mIsEditorObject; }
    float     GetPadding() { return mMaterial.mPadding; }


    Submesh* GetSubmesh() { return mSubmesh; }
  private:
    Material *mMaterialComponent;
    UBOMaterial mMaterial;
    size_t mIndex;
    std::string mDiffuseTexture;
    std::string mSpecularTexture;
    std::string mNormalTexture;

    Submesh *mSubmesh;
  };


  class Material : public Component
  {
  public:
    YTEDeclareType(Material);
    Material(Material&) = delete;

    Material(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Material() override;

    void Initialize() override;
    void Create(ModelChanged *aEvent);
    static std::vector<std::pair<YTE::Object*, std::string>> Lister(YTE::Object *aSelf);

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Model *mModel;

    MaterialRepresentation mModelMaterial;
    std::vector<std::unique_ptr<MaterialRepresentation>> mSubmeshMaterials;
  };
}

#endif
