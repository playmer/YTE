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
  class MaterialRepresentation : public Object
  {

  };


  class Material : public Component
  {
  public:
    YTEDeclareType(Material);
    Material(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Material() override;

    void Initialize() override;

  private:
    Renderer *mRenderer;
    Window *mWindow;

    
  };
}

#endif
