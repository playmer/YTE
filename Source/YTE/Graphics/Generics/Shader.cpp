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
    YTERegisterType(Shader);
  }



  Shader::Shader(std::string &aName)
    : mShaderSetName(aName)
  {
    Load();
  }



  void Shader::Load()
  {
    auto vertex = mShaderSetName + ".vert";
    auto fragment = mShaderSetName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());
    auto fragmentFile = Path::GetShaderPath(Path::GetEnginePath(), fragment.c_str());

    ReadFileToString(vertexFile, mVertexShaderCode);

    ReadFileToString(fragmentFile, mFragmentShaderCode);
  }
}
