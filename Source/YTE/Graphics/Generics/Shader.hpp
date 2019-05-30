#pragma once

#ifndef YTE_Graphics_Generics_Shader_hpp
#define YTE_Graphics_Generics_Shader_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  enum class ShaderStage
  {
    Compute,
    Tessellation,
    Vertex,
    Fragment
  };

  class Shader : public EventHandler
  {
  public:
    YTEDeclareType(Shader);

    Shader(std::string &aName);

    virtual void Reload() {}

    virtual ~Shader() = default;



  protected:
    std::string mShaderSetName;
  };
}

#endif
