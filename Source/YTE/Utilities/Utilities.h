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

#include "YTE/Utilities/String/String.h"

namespace YTE
{
  namespace filesystem = std::experimental::filesystem;

  extern std::wstring cWorkingDirectory;

  // Adapted from http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
  std::vector<std::string> split(const std::string &aString, char aDelimiter, bool aIgnoreEmpty);

  bool ReadFileToString(std::string &file, std::string &output);
  void StringToFloats(std::string &file, std::vector<float> &output);

  std::string Format(const char *aFormatString, ...);
  std::string Format(const char *aFormatString, va_list argv);
  
  std::string GetConfigPath(const String &name);
  std::string GetConfigPath(const char *name);
  std::string GetConfigPath(const std::string &name);

  std::string RemoveExtension(const std::string & filename);

  filesystem::path relativeTo(filesystem::path from, filesystem::path to);
}

#endif