#include "YTE/StandardLibrary/FileSystem.hpp"
#include <fstream>

#include "transcoder/basisu_transcoder.h"
#include "stb/stb_image.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace fs = std::filesystem;

namespace YTE
{
  basist::etc1_global_selector_codebook const* GetGlobalBasisCodebook()
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
  {
    YTEProfileFunction();
    Load(aFile);
  }

  

  Texture::Texture(const char *aFile)
  {
    YTEProfileFunction();
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
    YTEProfileFunction();
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

    std::vector<u8> dataToRead;
    if (!read_file_to_vec(mTexture.c_str(), dataToRead))
    {
      __debugbreak();
      return;
    }


    basist::basisu_transcoder transcoder{ GetGlobalBasisCodebook() };
    basist::basisu_file_info info;

    transcoder.validate_file_checksums(dataToRead.data(), dataToRead.size(), true);

    if (!transcoder.get_file_info(
      dataToRead.data(),
      dataToRead.size(),
      info))
    {
      __debugbreak();
      return;
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
      dataToRead.data(),
      dataToRead.size(),
      imageInfo,
      0))
    {
      __debugbreak();
      return;
    }

    if (!transcoder.start_transcoding(dataToRead.data(), dataToRead.size()))
    {
      __debugbreak();
      return;
    }

    auto constexpr transcoderFormat = basist::transcoder_texture_format::cTFBC3;
    auto const textureFormat = basist::basis_get_basisu_texture_format(transcoderFormat);

    basist::basisu_image_level_info levelInfo;

    if (!transcoder.get_image_level_info(
      dataToRead.data(),
      dataToRead.size(),
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
    auto const blocksX = (mWidth + mBlockWidth - 1) / mBlockWidth;
    auto const blocksY = (mHeight + mBlockHeight - 1) / mBlockHeight;

    auto const totalBlocks = blocksX * blocksY;

    auto const qwordsPerBlock = basisu::get_qwords_per_block(textureFormat);

    size_t const sizeInBytes = totalBlocks * qwordsPerBlock * sizeof(u64);

    // Zero out our data first.
    mData.resize(0);
    mData.resize(sizeInBytes);

    if (!transcoder.transcode_image_level(
      dataToRead.data(),
      dataToRead.size(),
      0,
      0,
      mData.data(),
      totalBlocks,
      transcoderFormat,
      0))
    {
      __debugbreak();
      return;
    }

    mType = TextureLayout::Bc3_Unorm;
  }
}
