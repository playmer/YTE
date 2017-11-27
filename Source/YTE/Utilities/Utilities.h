/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_Utilites_h
#define YTE_Utilities_Utilites_h

#include <vector>
#include <string>
#include <filesystem>

#include <bitset>

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Utilities/String/String.h"

namespace YTE
{
  namespace filesystem = std::experimental::filesystem;

  extern std::wstring cWorkingDirectory;

  // Version 4, Variant 1 GUID
  struct GlobalUniqueIdentifier
  {
    GlobalUniqueIdentifier();

    std::string ToString();

    bool operator==(GlobalUniqueIdentifier const& aGUID);


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
  std::vector<std::string> split(const std::string &aString, char aDelimiter, bool aIgnoreEmpty);

  bool ReadFileToString(std::string const &file, std::string &output);
  void StringToFloats(std::string &file, std::vector<float> &output);

  std::string Format(const char *aFormatString, ...);
  std::string Format(const char *aFormatString, va_list argv);
  
  std::string GetConfigPath(const String &name);
  std::string GetConfigPath(const char *name);
  std::string GetConfigPath(const std::string &name);
  bool FileCheck(const Path& aPath, const std::string& aDirectory, std::string &aFile);

  std::string RemoveExtension(const std::string & filename);

  filesystem::path relativeTo(filesystem::path from, filesystem::path to);
}

#endif