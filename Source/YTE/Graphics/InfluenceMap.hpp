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

#include "YTE/Physics/Transform.hpp"
#include "YTE/Platform/Keyboard.hpp"


namespace YTE
{
  class InfluenceMap : public Component
  {
  public:
    YTEDeclareType(InfluenceMap);

    InfluenceMap(Composition *aOwner, Space *aSpace, RSValue *aProperties);
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
    void SetCenter(glm::vec3& aCenter);
    void SetRadius(float aRadius);
    void SetColor(glm::vec3& aColor);

    glm::vec3 GetCenter() const;
    glm::vec3 GetColor() const;
    float GetRadius() const;

  private:
    void Create();
    void Destroy();

    Engine *mEngine;
    Renderer *mRenderer;
    GraphicsView *mGraphicsView;
    Transform *mTransform;
    std::unique_ptr<InstantiatedInfluenceMap> mInstantiatedInfluenceMap;
    bool mConstructing;
    UBOWaterInfluenceMap mMapTemp;
    bool mUseTemp;
    bool mSetTransform;
  };
}


#endif
