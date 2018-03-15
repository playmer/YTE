///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_Texture_hpp
#define YTE_Graphics_Generics_Texture_hpp

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

namespace YTE
{
  // Type of texture we are using
  enum class TextureType
  {
    RGBA,
    DXT1_sRGB,
    DXT5_sRGB
  };



  namespace TextureTypeIDs
  {
    extern const std::string Diffuse;
    extern const std::string Specular;
    extern const std::string Normal;
    extern const std::string FB1;
    extern const std::string FB2;
    extern const std::string FB3;
    extern const std::string FB4;
    extern const std::string FB5;
    extern const std::string FB6;
    extern const std::string Invalid;
  }



  enum class TextureViewType
  {
    e2D,
    eCube
  };



  struct TextureInformation
  {
    TextureInformation() : mFileName(TextureTypeIDs::Invalid),
                          mViewType(TextureViewType::e2D),
                          mTypeID(TextureTypeIDs::Invalid)
    {

    }

    TextureInformation(std::string aFileName, TextureViewType aViewType, std::string aTypeID)
      : mFileName(aFileName), mViewType(aViewType), mTypeID(aTypeID)
    {

    }

    std::string mFileName;
    TextureViewType mViewType;
    std::string mTypeID; // found in Texture.hpp->TextureTypeIDs::[...]
  };



  struct FrameBufferInformation
  {
    FrameBufferInformation() : mSampler(nullptr),
                           mImage(nullptr),
                           mViewType(TextureViewType::e2D),
                           mTypeID(TextureTypeIDs::Invalid)
    {

    }

    FrameBufferInformation(std::shared_ptr<vkhlf::Sampler>* aSampler, std::shared_ptr<vkhlf::ImageView>* aImage, TextureViewType aViewType, std::string aTypeID)
      : mSampler(aSampler), mImage(aImage), mViewType(aViewType), mTypeID(aTypeID)
    {
      
    }

    std::shared_ptr<vkhlf::Sampler>* mSampler;
    std::shared_ptr<vkhlf::ImageView>* mImage;
    TextureViewType mViewType;
    std::string mTypeID; // found in Texture.hpp->TextureTypeIDs::[...]
  };



  struct Texture : public EventHandler
  {
  public:
    YTEDeclareType(Texture);

    Texture(std::string &aFile);
    Texture(const char *aFile);

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
