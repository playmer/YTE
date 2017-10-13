#pragma once

#include "YTE/Core/Utilities.hpp"
#include "YTE/Core/PrivateImplementation.hpp"

namespace YTE
{
  enum class TextureType
  {
    RGBA,
    DXT1_sRGB,
    DXT5_sRGB
  };

  struct Texture
  {
    Texture(std::string &aFile);

    u32 mWidth;
    u32 mHeight;
    u32 mMipLevels;
    u32 mLayerCount;

    std::vector<u8> mData;

    std::string mTexturePath;
    std::string mTextureFileName;

    TextureType mType;

    PrivateImplementationDynamic mRendererData;
  };
}
