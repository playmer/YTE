#pragma once

namespace YTE::Tools 
{
  struct TextureImportSettings
  {
    // 1 - 5, 5 being slowest.
    int CompressionLevel = 255;
    // 1- 255, 255 is best.
    int QualityLevel = 255;
    
    // Turn this on if the texture is a normal map.
    bool NormalMap = false;

    // Turn this on if the texture isn't sRGB (thus it's linear)
    bool LinearSpaceTexture = false;
  };

  std::vector<u8> ImportTexture(std::string const& aFile, TextureImportSettings aTextureImporting);
}