#include "YTE/StandardLibrary/FileSystem.hpp"

#include "YTE/Core/AssetLoader.hpp"

#include <cstdio>

namespace YTE
{
  namespace fs = std::filesystem;

  Path::Path(std::string aPath) : mPath(aPath)
  {
  }

  struct PathHelper
  {
    non_member Path MakePath(const std::string& aPath)
    {
      return Path(aPath);
    }
  };

  namespace
  {
    intern Path sGamePath = PathHelper::MakePath("");
    intern Path sEnginePath = PathHelper::MakePath("");
  }

  // TODO: Support unicode file aPaths ... eventually ... maybe
  const Path& Path::SetGamePath(std::string aPath)
  {
    fs::path path{ aPath };

    if (true == path.has_stem())
    {
      path.append("");
    }

    sGamePath = Path(path.u8string());
    return sGamePath;
  }

  const Path& Path::SetEnginePath(std::string aPath)
  {
    fs::path path{ aPath };

    if (true == path.has_stem())
    {
      path.append("");
    }

    sEnginePath = Path(path.u8string());
    return sEnginePath;
  }

  const Path& Path::GetGamePath()
  {
    return sGamePath;
  }

  const Path& Path::GetEnginePath()
  {
    return sEnginePath;
  }

  std::string Path::GetArchetypePath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Archetypes");
    path.append(aName);
    path.concat(".json");

    return path.string();
  }

  std::string Path::GetLevelPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Levels");
    path.append(aName);
    path.concat(".json");

    return path.string();
  }

  std::string Path::GetShaderPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Shaders");
    path.append(aName);

    return path.string();
  }

  std::string Path::GetModelPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Models");
    path.append(aName);

    return path.string();
  }
  
  std::string Path::GetSkeletonPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Skeletons");
    path.append(aName);

    return path.string();
  }

  std::string Path::GetAnimationPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Animations");
    path.append(aName);

    return path.string();
  }

  std::string Path::GetTexturePath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Textures");
    path.append(aName);

    return path.u8string();
  }

  std::string Path::GetTextsDirectory(const Path& aPath)
  {
    fs::path path{ aPath.mPath };
    path.append("Texts");

    return path.string();
  }

  std::string Path::GetTextPath(const Path& aPath, const std::string &aName)
  {
    fs::path path{ aPath.mPath };
    path.append("Texts");
    path.append(aName);

    return path.string();
  }

  const std::string& Path::String() const
  {
    return mPath;
  }

  bool operator!=(const Path& aLeft, const Path& aRight)
  {
    return aLeft.mPath != aRight.mPath;
  }

  bool operator==(const Path& aLeft, const Path& aRight)
  {
    return aLeft.mPath == aRight.mPath;
  }

  intern std::vector<byte> LoadBinaryFile(const std::string& aPath)
  {
    const size_t cPageSize = 4096;
    std::vector<byte> data;
    FILE *file = fopen(aPath.c_str(), "rb");
    if (file)
    {
      size_t bytesRead = 0;
      do
      {
        data.resize(data.size() + cPageSize, 0);
        bytesRead = fread(&data.at(data.size()), sizeof(byte), cPageSize, file);
      } while (bytesRead != cPageSize || feof(file));

      data.resize(data.size() - (cPageSize - bytesRead));
    }

    return data;
  }

  std::shared_ptr<Asset> AssetLoader::LoadAsset(Asset::Type aType, const Path& aDirectory, std::string aFileName)
  {
    auto& mapContainer = mAssets[EnumCast(aType)];
    auto assetIt = mapContainer.find(aFileName);
    if (assetIt != mapContainer.end())
    {
      return assetIt->second;
    }
    else
    {
      std::string file;
      switch (aType)
      {
        case Asset::Type::Archetype:
        {
          file = Path::GetArchetypePath(aDirectory, aFileName);
        } break;
        case Asset::Type::Level:
        {
          file = Path::GetLevelPath(aDirectory, aFileName);
        } break;
        case Asset::Type::Shader:
        {
          file = Path::GetShaderPath(aDirectory, aFileName);
        } break;
        case Asset::Type::Mesh:
        {
          file = Path::GetModelPath(aDirectory, aFileName);
        } break;
        case Asset::Type::Texture:
        {
          file = Path::GetTexturePath(aDirectory, aFileName);
        } break;
        case Asset::Type::Text:
        {
          file = Path::GetTextPath(aDirectory, aFileName);
        } break;
        default:
        {
          DebugObjection(true, "That type isn't supported?");
        } break;
      }
      std::vector<byte> data = LoadBinaryFile(file);
      mapContainer.insert(std::make_pair(aFileName, std::make_shared<Asset>(aDirectory, aFileName, aType, data)));

      return nullptr;
    }
  }

  // TODO(Evelyn): Make this stage files to be unloaded instead of doing it right away <3
  bool AssetLoader::UnloadAsset(std::shared_ptr<Asset> aAsset)
  {
    auto &mapContainer = mAssets[EnumCast(aAsset->GetType())];
    if (aAsset.use_count() < 3)
    {
      auto it = mapContainer.find(aAsset->GetName());
      if (it != mapContainer.end())
      {
        mapContainer.erase(aAsset->GetName());
        return true;
      }
    }
    return false;
  }
}