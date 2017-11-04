///////////////////
// Author: Joshua T. Fisher
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedSprite_hpp
#define YTE_Graphics_Generics_InstantiatedSprite_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  class InstantiatedSprite : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedSprite);

    virtual ~InstantiatedSprite() = default;

    virtual void UpdateUBOModel(UBOModel &aUBO)
    {
      YTEUnusedArgument(aUBO);
    }

  protected:
  };
}

#endif
