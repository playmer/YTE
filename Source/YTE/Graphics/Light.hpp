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

    Light(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Light() override;
    void Initialize() override; 

    
    
    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    void TransformUpdate(TransformChanged *aEvent);
    void Update(LogicUpdate* aEvent);

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    void SetLightSourceInformation(UBOLight &aLight);
    void SetPosition(glm::vec3& aPosition);
    void SetDirection(glm::vec3& aDirection);
    void SetAmbient(glm::vec3& aColor);
    void SetDiffuse(glm::vec3& aColor);
    void SetSpecular(glm::vec3& aColor);
    void SetSpotLightCones(glm::vec2& aCones);  // inner, outer
    void SetLightType(std::string aLightType);
    void SetSpotLightFalloff(float& aFalloff);
    void SetActive(bool aValue);
    void SetIntensity(float& aItensity);

    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;
    glm::vec3 GetAmbient() const;
    glm::vec3 GetDiffuse() const;
    glm::vec3 GetSpecular() const;
    glm::vec2 GetSpotLightCones() const;
    std::string GetLightType() const;
    float GetSpotLightFalloff() const;
    bool GetActive() const;
    float GetIntensity() const;



  private:
    void Create();
    void Destroy();

    Engine *mEngine;
    Renderer *mRenderer;
    GraphicsView *mGraphicsView;
    Transform *mTransform;
    std::unique_ptr<InstantiatedLight> mInstantiatedLight;
    bool mConstructing;
    UBOLight mLightTemp;
    bool mUseTemp;
    bool mSetTransform;
  };
}


#endif
