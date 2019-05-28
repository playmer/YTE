#pragma once

#ifndef YTE_Graphics_Generics_Texture_hpp
#define YTE_Graphics_Generics_Texture_hpp

#include "YTE/Core/EventHandler.hpp"

namespace YTE
{
  enum class TextureLayout
  {
    RGBA,
    Bc1_Rgba_Srgb,
    Bc3_Srgb,
    Bc3_Unorm
  };

  enum class TextureType
  {
    e1D,
    e2D,
    e3D,
    eCube,
    e1DArray,
    e2DArray,
    eCubeArray
  };



  struct Texture : public EventHandler
  {
  public:
    YTEDeclareType(Texture);

    Texture(const std::string &aFile);
    Texture(const char *aFile);

    Texture(std::vector<u8> aData, 
            TextureLayout aType, 
            u32 aWidth, 
            u32 aHeight, 
            u32 aMipLevels, 
            u32 aLayerCount)
      : mData{ std::move(aData) }
      , mWidth{ aWidth }
      , mHeight{ aHeight }
      , mMipLevels{ aMipLevels }
      , mLayerCount{ aLayerCount }
      , mBlockWidth{ 0 }
      , mBlockHeight{ 0 }
      , mBytesPerBlock{ 0 }
      , mType{ aType }
    {

    }

    virtual ~Texture()
    {
      
    }


    std::vector<u8> mData;
    std::string mTexture;

    u32 mWidth;
    u32 mHeight;
    u32 mMipLevels;
    u32 mLayerCount;
    u32 mBlockWidth;
    u32 mBlockHeight;
    u32 mBytesPerBlock;
    TextureLayout mType;

  protected:
    void Load(std::string const& aFile);
  };
}


#endif
