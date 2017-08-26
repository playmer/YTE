#include <filesystem>
#include <fstream>

//#include "crunch/inc/crnlib.h"
#include "crunch/inc/crn_decomp.h"
#include "stb/stb_image.h"

#include "YTE/Graphics/Texture.hpp"

#include "YTE/Utilities/Utilities.h"


namespace fs = std::experimental::filesystem;

namespace YTE
{
  Texture::Texture(std::string &aFile)
  {
    auto textureFile = GetTexture(aFile);

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
      DebugAssert(texWidth <= 0,  
                  "Texture: %s, has negative width!",  
                  aFile.c_str()); 
 
      DebugAssert(texHeight <= 0, 
                  "Texture: %s, has negative height!",  
                  aFile.c_str()); 

      mWidth = static_cast<u32>(texWidth);
      mHeight = static_cast<u32>(texHeight);

      //DebugAssert(texChannels < 4, 
      //            "Texture: %s, has only %d channels, we require 4 (RGBA)", 
      //            aFile.c_str(), 
      //            texChannels);
      DebugAssert(loadedFile == nullptr,
                  "Failed to load texture: %s", 
                  aFile.c_str());

      size_t size = mWidth * mHeight * STBI_rgb_alpha;
      mData.resize(size);

      memcpy(mData.data(), loadedFile, size);

      mType = TextureType::RGBA;
    }
    else if (type == L".crn")
    {
      std::ifstream file(textureFile, std::ios::binary | std::ios::ate);
      std::streamsize streamSize = file.tellg();
      file.seekg(0, std::ios::beg);
      mData.resize(streamSize, 0);

      if (file.read(reinterpret_cast<char*>(mData.data()), streamSize).bad())
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
