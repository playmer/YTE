///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Shader.hpp"

namespace YTE
{
  YTEDefineType(Shader)
  {
    RegisterType<Shader>();
    TypeBuilder<Shader> builder;
  }



  Shader::Shader(std::string &aName)
    : mShaderSetName(aName)
  {
  }
}
