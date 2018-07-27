///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_Light_hpp
#define YTE_Graphics_Light_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/InstantiatedLight.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Platform/Keyboard.hpp"


namespace YTE
{
  class Light : public Component
  {
  public:
    YTEDeclareType(Light);

    YTE_Shared Light(Composition *aOwner, Space *aSpace);
    YTE_Shared ~Light() override;
    YTE_Shared void Initialize() override; 

    
    
    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    YTE_Shared void TransformUpdate(TransformChanged *aEvent);
    YTE_Shared void Update(LogicUpdate* aEvent);

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    YTE_Shared void SetLightSourceInformation(UBOLight &aLight);
    YTE_Shared void SetPosition(glm::vec3& aPosition);
    YTE_Shared void SetDirection(glm::vec3& aDirection);
    YTE_Shared void SetAmbient(glm::vec3& aColor);
    YTE_Shared void SetDiffuse(glm::vec3& aColor);
    YTE_Shared void SetSpecular(glm::vec3& aColor);
    YTE_Shared void SetSpotLightCones(glm::vec2& aCones);  // inner, outer
    YTE_Shared void SetLightType(std::string aLightType);
    YTE_Shared void SetSpotLightFalloff(float& aFalloff);
    YTE_Shared void SetActive(bool aValue);
    YTE_Shared void SetIntensity(float& aItensity);

    YTE_Shared glm::vec3 GetPosition() const;
    YTE_Shared glm::vec3 GetDirection() const;
    YTE_Shared glm::vec3 GetAmbient() const;
    YTE_Shared glm::vec3 GetDiffuse() const;
    YTE_Shared glm::vec3 GetSpecular() const;
    YTE_Shared glm::vec2 GetSpotLightCones() const;
    YTE_Shared std::string GetLightType() const;
    YTE_Shared float GetSpotLightFalloff() const;
    YTE_Shared bool GetActive() const;
    YTE_Shared float GetIntensity() const;



  private:
    void Create();
    void Destroy();

    Engine *mEngine;
    Renderer *mRenderer;
    GraphicsView *mGraphicsView;
    Transform *mTransform;
    std::unique_ptr<InstantiatedLight> mInstantiatedLight;
    UBOLight mLightTemp;
    bool mUseTemp;
    bool mSetTransform;
  };
}


#endif
