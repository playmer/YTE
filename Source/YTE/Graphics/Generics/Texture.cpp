#include "YTE/StandardLibrary/FileSystem.hpp"
#include <fstream>

#include "transcoder/basisu_transcoder.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace fs = std::filesystem;

namespace YTE
{
  static basist::etc1_global_selector_codebook const* GetGlobalBasisCodebook()
  {
    using namespace basist;
    static basist::etc1_global_selector_codebook codebook(g_global_selector_cb_size, g_global_selector_cb);

    return &codebook;
  }

  YTEDefineType(Texture)
  {
    RegisterType<Texture>();
    TypeBuilder<Texture> builder;
  }

  Texture::Texture(std::string const&aFile) 
    : mType{TextureLayout::InvalidLayout}
  {
    OPTICK_EVENT();
    Load(aFile);
  }

  Texture::Texture(const char *aFile) 
    : mType{ TextureLayout::InvalidLayout }
  {
    OPTICK_EVENT();
    std::string f(aFile);
    Load(f);
  }

  bool read_file_to_vec(const char* pFilename, std::vector<u8>& data)
  {
    FILE* pFile = nullptr;
#ifdef _WIN32
    fopen_s(&pFile, pFilename, "rb");
#else
    pFile = fopen(pFilename, "rb");
#endif
    if (!pFile)
      return false;

    fseek(pFile, 0, SEEK_END);
#ifdef _WIN32
    int64_t filesize = _ftelli64(pFile);
#else
    int64_t filesize = ftello(pFile);
#endif
    if (filesize < 0)
    {
      fclose(pFile);
      return false;
    }
    fseek(pFile, 0, SEEK_SET);

    if (sizeof(size_t) == sizeof(uint32_t))
    {
      if (filesize > 0x70000000)
      {
        // File might be too big to load safely in one alloc
        fclose(pFile);
        return false;
      }
    }

    data.resize((size_t)filesize);

    if (filesize)
    {
      if (fread(&data[0], 1, (size_t)filesize, pFile) != (size_t)filesize)
      {
        fclose(pFile);
        return false;
      }
    }

    fclose(pFile);
    return true;
  }


  //bool read_file_to_vec(const char* pFilename, std::vector<u8>& data)
  //{
  //
  //}



  void Texture::Load(std::string const& aFile)
  {
    OPTICK_EVENT();
    fs::path filePath{ aFile };

    auto extension = filePath.extension();

    if (".basis" != extension)
    {
      filePath = filePath.filename().stem().concat(".basis");
    }

    auto file = filePath.u8string();
    mTexture = Path::GetTexturePath(Path::GetEnginePath(), file);

    if (false == fs::exists(mTexture))
    {
      mTexture = fs::path(Path::GetTexturePath(Path::GetGamePath(), file)).u8string();
    }

    // TODO (Josh): mmap?
    //auto const dataToRead = ReadFileToVector(textureFile);

    if (!read_file_to_vec(mTexture.c_str(), mData))
    {
      __debugbreak();
      return;
    }

    Decode();
  }

  void Texture::Decode()
  {
    OPTICK_EVENT();

    if (TextureLayout::RGBA != mType)
    {
      mType = TextureLayout::Bc7_Unorm_Opaque;

      basist::basisu_transcoder transcoder{ GetGlobalBasisCodebook() };
      basist::basisu_file_info info;

      if (!transcoder.validate_file_checksums(
        mData.data(),
        static_cast<u32>(mData.size()),
        true))
      {
        __debugbreak();
        return;
      }

      if (!transcoder.get_file_info(
        mData.data(),
        static_cast<u32>(mData.size()),
        info))
      {
        __debugbreak();
        return;
      }

      if (info.m_has_alpha_slices)
      {
        mType = TextureLayout::Bc3_Unorm;
      }

      if (
        1 != info.m_image_mipmap_levels.size() ||
        1 != info.m_total_images)
      {
        __debugbreak();
        return;
      }

      mMipLevels = info.m_image_mipmap_levels[0];

      basist::basisu_image_info imageInfo;
      if (!transcoder.get_image_info(
        mData.data(),
        static_cast<u32>(mData.size()),
        imageInfo,
        0))
      {
        __debugbreak();
        return;
      }

      auto constexpr transcoderFormat = basist::transcoder_texture_format::cTFBC3;
      auto const textureFormat = basist::basis_get_basisu_texture_format(transcoderFormat);

      basist::basisu_image_level_info levelInfo;

      if (!transcoder.get_image_level_info(
        mData.data(),
        static_cast<u32>(mData.size()),
        levelInfo,
        0,
        0))
      {
        __debugbreak();
        return;
      }

      mWidth = levelInfo.m_orig_width;
      mHeight = levelInfo.m_orig_height;

      mBytesPerBlock = basisu::get_bytes_per_block(textureFormat);

      mBlockWidth = basisu::get_block_width(textureFormat);
      mBlockHeight = basisu::get_block_height(textureFormat);

    }


    // FIX

    switch (mType)
    {
      case TextureLayout::Bc1_Rgba_Srgb:
      case TextureLayout::Bc3_Srgb:
      case TextureLayout::Bc3_Unorm:
      case TextureLayout::Bc7_Unorm_Opaque:
      {
        std::vector<u8> decodedTexture;

        auto const height = mHeight;
        auto const width = mWidth;

        auto const basisRowPitch = mBytesPerBlock * ((width + 3) / 4);

        basist::basisu_transcoder transcoder{ GetGlobalBasisCodebook() };
        basist::basisu_file_info info;

        auto transcoderFormat = basist::transcoder_texture_format::cTFBC7_M6_OPAQUE_ONLY;

        if (TextureLayout::Bc3_Unorm == mType ||
          TextureLayout::Bc3_Srgb == mType)
        {
          transcoderFormat = basist::transcoder_texture_format::cTFBC3;
        }

        auto const textureFormat = basist::basis_get_basisu_texture_format(transcoderFormat);

        auto const blocksX = (mWidth + mBlockWidth - 1) / mBlockWidth;
        auto const blocksY = (mHeight + mBlockHeight - 1) / mBlockHeight;

        auto const totalBlocks = blocksX * blocksY;

        auto const qwordsPerBlock = basisu::get_qwords_per_block(textureFormat);

        size_t const sizeInBytes = totalBlocks * qwordsPerBlock * sizeof(u64);
        
        decodedTexture.resize(sizeInBytes);
        uint8_t* dataWriter = decodedTexture.data();

        if (!transcoder.start_transcoding(
          mData.data(),
          static_cast<u32>(mData.size())))
        {
          __debugbreak();
          return;
        }

        if (!transcoder.transcode_image_level(
          mData.data(),
          static_cast<u32>(mData.size()),
          0,
          0,
          dataWriter,
          totalBlocks,
          transcoderFormat,
          0,
          0/*static_cast<u32>(layout.rowPitch / basist::basis_get_bytes_per_block(transcoderFormat))*/))
        {
          __debugbreak();
          return;
        }

        mData = decodedTexture;

        break;
      }
    }
  }
}
