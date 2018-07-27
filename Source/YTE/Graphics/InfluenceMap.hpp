///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_InfluenceMap_hpp
#define YTE_Graphics_InfluenceMap_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/InstantiatedInfluenceMap.hpp"
#include "YTE/Graphics/UBOs.hpp"
#include "YTE/Graphics/Drawers.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Platform/Keyboard.hpp"


namespace YTE
{
  class InfluenceMap : public Component
  {
  public:
    YTEDeclareType(InfluenceMap);

    InfluenceMap(Composition *aOwner, Space *aSpace);
    ~InfluenceMap() override;
    void Initialize() override; 

    
    
    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    void TransformUpdate(TransformChanged *aEvent);
    void Update(LogicUpdate* aEvent);

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    void SetMapSourceInformation(UBOWaterInfluenceMap &aMap);
    void SetRadius(float aRadius);
    void SetColor(glm::vec3& aColor);
    void SetDebugDraw(bool aDraw);
    void SetColorIntensity(float aIntensity);
    void SetWaveIntensity(float aIntensity);
    void SetActive(bool aActive);
    void SetColorInfluenceFunction(std::string aFunction);
    void SetWaveInfluenceFunction(std::string aFunction);

    glm::vec3 GetColor() const;
    float GetRadius() const;
    float GetColorIntensity() const;
    float GetWaveIntensity() const;
    bool GetActive() const;
    std::string GetColorInfluenceFunction() const;
    std::string GetWaveInfluenceFunction() const;
    bool GetDebugDraw() const
    {
      return mDebugDraw;
    }


  private:
    void Create();
    void Destroy();

    Engine *mEngine;
    Renderer *mRenderer;
    GraphicsView *mGraphicsView;
    Transform *mTransform;
    std::unique_ptr<InstantiatedInfluenceMap> mInstantiatedInfluenceMap;
    UBOWaterInfluenceMap mMapTemp;
    bool mUseTemp;
    bool mSetTransform;
    std::unique_ptr<LineDrawer> mDrawer;
    bool mDebugDraw;
  };
}


#endif
