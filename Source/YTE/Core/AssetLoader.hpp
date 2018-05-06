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
      YTE_Shared non_member const Path& SetGamePath(std::string aPath);
      YTE_Shared non_member const Path& SetEnginePath(std::string aPath);

      YTE_Shared non_member const Path& GetGamePath();
      YTE_Shared non_member const Path& GetEnginePath();

      YTE_Shared non_member std::string GetArchetypePath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetLevelPath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetShaderPath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetModelPath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetAnimationPath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetTexturePath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetWWisePath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetTextPath(const Path& aPath, const std::string &aName);
      YTE_Shared non_member std::string GetTextsDirectory(const Path& aPath);

      YTE_Shared const std::string& String() const;

      friend class AssetLoader;
      friend struct PathHelper;
      YTE_Shared friend bool operator!=(const Path& aLeft, const Path& aRight);
      YTE_Shared friend bool operator==(const Path& aLeft, const Path& aRight);
  };

  

  class AssetLoader
  {
    private:
      std::array<std::map<std::string, std::shared_ptr<Asset>>, EnumCast(Asset::Type::_Count)>  mAssets;
    public:
      YTE_Shared std::shared_ptr<Asset> LoadAsset(Asset::Type aType, const Path& aDirectory, std::string aFileNamee);
      YTE_Shared bool UnloadAsset(std::shared_ptr<Asset> aAsset);
  };
  
}