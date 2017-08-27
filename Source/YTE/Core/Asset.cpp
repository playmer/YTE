#include "YTE/Core/Asset.hpp"
#include "YTE/Core/AssetLoader.hpp"

namespace YTE
{
  Asset::Asset(const Path& aPath, const std::string& aName, Asset::Type aType, std::vector<byte> aData)
    : mPath(aPath)
    , mName(aName)
    , mType(aType)
    , mData(std::move(aData))
  {
  }

  const std::vector<byte> Asset::GetData() const
  {
    return mData;
  }

  Asset::Type Asset::GetType() const
  {
    return mType;
  }

  const std::string& Asset::GetName() const
  {
    return mName;
  }

  bool Asset::FromEngine() const
  {
    return Path::GetEnginePath() == mPath;
  }

  bool Asset::FromGame() const
  {
    return Path::GetGamePath() == mPath;
  }
}