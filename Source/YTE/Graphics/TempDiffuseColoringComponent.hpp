#pragma once
#ifndef YTE_Graphics_TempDiffuseColoringComponent_hpp 
#define YTE_Graphics_TempDiffuseColoringComponent_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class TempDiffuseColoringComponent : public Component
  {
  public:
    YTEDeclareType(TempDiffuseColoringComponent);
    TempDiffuseColoringComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~TempDiffuseColoringComponent();
    void Initialize() override;
    void SetDiffuseColor(glm::vec3 aColor);

    glm::vec3 GetDiffuseColor()
    {
      return mDiffuseColor;
    }


  private:
    Model * mModel;
    glm::vec3 mDiffuseColor;
    bool update;
  };

}

#endif