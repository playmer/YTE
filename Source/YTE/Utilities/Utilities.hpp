#pragma once

#ifndef YTE_Utilities_Utilites_h
#define YTE_Utilities_Utilites_h

#include <vector>
#include <string>
#include "YTE/StandardLibrary/FileSystem.hpp"

#include <bitset>

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Utilities/String/String.hpp"

namespace YTE
{
  namespace filesystem = std::filesystem;

  YTE_Shared extern std::wstring cWorkingDirectory;

  // Version 4, Variant 1 GUID
  struct GlobalUniqueIdentifier
  {
    YTE_Shared GlobalUniqueIdentifier();

    YTE_Shared std::string ToString() const;
    YTE_Shared std::string ToIdentifierString() const;

    YTE_Shared bool operator==(GlobalUniqueIdentifier const& aGUID);


    //       u32           u16         u16        u16            u32         u16
    //(xx)(xx)(xx)(xx) - (xx)(xx) - (Mx)(xx) - (Nx)(xx) - (xx)(xx)(xx)(xx)(xx)(xx)
    u32 mPart1;
    u16 mPart2;
    u16 mVersion;
    u16 mVariant;
    u32 mPart3;
    u16 mPart4;
  };

  // Adapted from http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
  YTE_Shared std::vector<std::string> split(const std::string &aString, char aDelimiter, bool aIgnoreEmpty);

  YTE_Shared bool ReadFileToString(std::string const &file, std::string &output);
  YTE_Shared void StringToFloats(std::string &file, std::vector<float> &output);

  YTE_Shared std::string Format(const char *aFormatString, ...);
  YTE_Shared std::string Format(const char *aFormatString, va_list argv);
  
  YTE_Shared std::string GetConfigPath(const String &name);
  YTE_Shared std::string GetConfigPath(const char *name);
  YTE_Shared std::string GetConfigPath(const std::string &name);
  YTE_Shared bool FileCheck(const Path& aPath, const std::string& aDirectory, std::string &aFile);

  YTE_Shared std::string RemoveExtension(const std::string & filename);

  YTE_Shared filesystem::path relativeTo(filesystem::path from, filesystem::path to);
}

#endif