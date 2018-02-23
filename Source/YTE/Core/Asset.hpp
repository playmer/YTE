#pragma once

#include "YTE/Core/Utilities.hpp"

namespace YTE
{

  class Path;

  class Asset
  {
  public:
    enum class Type : unsigned char
    {
      Archetype,
      Level,
      Shader,
      Mesh,
      Texture,
      Text,

      _Count
    };

    explicit Asset(const Path& aPath, const std::string& aName, Asset::Type aType, std::vector<byte> aData);
    //explicit Asset(const Path& aPath, Type aType, std::vector<byte> aData);
    const std::vector<byte> GetData() const;
    Type GetType() const;
    const std::string& GetName() const;
    bool FromEngine() const;
    bool FromGame() const;

  private:
    const std::string& mName;
    const Path& mPath;
    std::vector<byte> mData;
    Type mType;
  };
}