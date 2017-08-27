#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

#include "YTE/Core/Asset.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  class Path
  {
    private:
      explicit Path(std::string aPath);
      std::string mPath;

    public:
      non_member const Path& SetGamePath(std::string aPath);
      non_member const Path& SetEnginePath(std::string aPath);

      non_member const Path& GetGamePath();
      non_member const Path& GetEnginePath();

      non_member std::string GetArchetypePath(const Path& aPath, const std::string &aName);
      non_member std::string GetLevelPath(const Path& aPath, const std::string &aName);
      non_member std::string GetShaderPath(const Path& aPath, const std::string &aName);
      non_member std::string GetModelPath(const Path& aPath, const std::string &aName);
      non_member std::string GetTexturePath(const Path& aPath, const std::string &aName);

      const std::string& String() const;

      friend class AssetLoader;
      friend struct PathHelper;
      friend bool operator!=(const Path& aLeft, const Path& aRight);
      friend bool operator==(const Path& aLeft, const Path& aRight);
  };

  

  class AssetLoader
  {
    private:
      std::array<std::map<std::string, std::shared_ptr<Asset>>, enum_cast(Asset::Type::_Count)>  mAssets;
    public:
      std::shared_ptr<Asset> LoadAsset(Asset::Type aType, const Path& aDirectory, std::string aFileNamee);
      bool UnloadAsset(std::shared_ptr<Asset> aAsset);
  };
  
}