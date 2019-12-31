#include "YTE/StandardLibrary/FileSystem.hpp"
#include <fstream>

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Utilities/Utilities.hpp"


namespace fs = std::filesystem;

namespace YTE
{
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
  }
}
