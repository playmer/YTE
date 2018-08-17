#pragma once

#include <vector>

#include "YTE/Graphics/DirectX12/DX12FunctionLoader.hpp"

namespace YTE
{
  struct SpirV
  {
    SpirV()
      : mReason("We haven't actually set the data.")
      , mValid(false)
    {

    }

    std::vector<uint32_t> mData;
    std::string mReason;
    bool mValid;
  };

  SpirV CompileGLSLToSPIRV(vk::ShaderStageFlagBits stage, 
                           std::string const &aFile, 
                           std::string &aDefines,
                           bool aFilenameIsShaderText);
}
