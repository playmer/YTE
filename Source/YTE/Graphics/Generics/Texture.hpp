///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Texture_hpp
#define YTE_Graphics_Generics_Texture_hpp

#include "YTE/Core/EventHandler.hpp"

namespace YTE
{
  enum class TextureLayout
  {
    RGBA,
    DXT1_sRGB,
    DXT5_sRGB
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
      : mWidth{aWidth}
      , mHeight(aHeight)
      , mMipLevels(aMipLevels)
      , mLayerCount(aLayerCount)
      , mData{ std::move(aData) }
      , mType{aType}
    {

    }

    virtual ~Texture()
    {
      
    }

    u32 mWidth;
    u32 mHeight;
    u32 mMipLevels;
    u32 mLayerCount;

    std::vector<u8> mData;

    std::string mTexturePath;
    std::string mTextureFileName;

    TextureLayout mType;



  protected:
    void Load(std::string &aFile);
  };
}


#endif
