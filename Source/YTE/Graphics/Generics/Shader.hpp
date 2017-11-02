///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Shader_hpp
#define YTE_Graphics_Generics_Shader_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  class Shader : public EventHandler
  {
  public:
    YTEDeclareType(Shader);

    Shader(std::string &aName);

    virtual ~Shader() = default;



  protected:
    void Load();

    std::string mShaderSetName;
    std::string mVertexShaderCode;
    std::string mFragmentShaderCode;
  };
}

#endif
