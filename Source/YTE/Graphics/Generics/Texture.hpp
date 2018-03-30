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
  // Type of texture we are using
  enum class TextureType
  {
    RGBA,
    DXT1_sRGB,
    DXT5_sRGB
  };



  struct Texture : public EventHandler
  {
  public:
    YTEDeclareType(Texture);

    Texture(std::string &aFile);
    Texture(const char *aFile);

    Texture(std::vector<u8> aData, 
            TextureType aType, 
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

    TextureType mType;



  protected:
    void Load(std::string &aFile);
  };
}


#endif
