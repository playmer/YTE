///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include <filesystem>
#include <fstream>

//#include "crunch/inc/crnlib.h"
#include "crunch/inc/crn_decomp.h"
#include "stb/stb_image.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Utilities/Utilities.h"


namespace fs = std::experimental::filesystem;

namespace YTE
{
  YTEDefineType(Texture)
  {
    YTERegisterType(Texture);
  }



  Texture::Texture(std::string &aFile)
  {
    Load(aFile);
  }

  

  Texture::Texture(const char *aFile)
  {
    std::string f(aFile);
    Load(f);
  }



  void Texture::Load(std::string &aFile)
  {
    fs::path file{ aFile };

    std::string textureName{ file.stem().string() };

    //TODO (Josh): Make Crunch work.
    //file = L"Crunch" / file.filename().concat(L".crn");
    file = L"Originals" / file.filename();
    std::string fileStr{ file.string() };

    auto textureFile = Path::GetTexturePath(Path::GetGamePath(), fileStr);

    fs::path type{ textureFile };
    type = type.extension();

    if (type == L".png")
    {
      int texWidth, texHeight, texChannels;

      stbi_uc* loadedFile = stbi_load(textureFile.c_str(),
        &texWidth,
        &texHeight,
        &texChannels,
        STBI_rgb_alpha);
      DebugObjection(texWidth <= 0,
        "Texture: %s, has negative width!",
        aFile.c_str());

      DebugObjection(texHeight <= 0,
        "Texture: %s, has negative height!",
        aFile.c_str());

      mWidth = static_cast<u32>(texWidth);
      mHeight = static_cast<u32>(texHeight);

      //DebugObjection(texChannels < 4, 
      //            "Texture: %s, has only %d channels, we require 4 (RGBA)", 
      //            aFile.c_str(), 
      //            texChannels);
      DebugObjection(loadedFile == nullptr,
        "Failed to load texture: %s",
        aFile.c_str());

      size_t size = mWidth * mHeight * STBI_rgb_alpha;
      mData.resize(size);

      memcpy(mData.data(), loadedFile, size);
      stbi_image_free(loadedFile),

        mType = TextureType::RGBA;
    }
    else if (type == L".crn")
    {
      std::ifstream fileToRead(textureFile, std::ios::binary | std::ios::ate);
      std::streamsize streamSize = fileToRead.tellg();
      fileToRead.seekg(0, std::ios::beg);
      mData.resize(streamSize, 0);

      if (fileToRead.read(reinterpret_cast<char*>(mData.data()), streamSize).bad())
      {
        return;
      }

      crnd::crn_texture_info info;
      info.m_struct_size = sizeof(crnd::crn_texture_info);

      if (false == crnd_get_texture_info(mData.data(), static_cast<u32>(mData.size()), &info))
      {
        return;
      }

      u32 ddsSize;
      auto dds = crn_decompress_crn_to_dds(mData.data(), ddsSize);

      mData.clear();
      mData.resize(ddsSize);
      memcpy(mData.data(), dds, ddsSize);

      // TODO (Josh): Be sure this is safe.
      free(dds);

      mWidth = info.m_width;
      mHeight = info.m_height;

      switch (info.m_format)
      {
        case cCRNFmtDXT1:
        {
          mType = TextureType::DXT1_sRGB;
          break;
        }
        case cCRNFmtDXT5:
        {
          mType = TextureType::DXT5_sRGB;
          break;
        }
      }
    }
  }

}
