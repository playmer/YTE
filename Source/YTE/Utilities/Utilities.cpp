/*****************************************************************************
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
\author Joshua T. Fisher
******************************************************************************/
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <memory>
#include <cstdlib>
#include <filesystem>
#include <stdarg.h> /* va_list, va_start, va_end*/
#include <stdio.h>


#include "YTE/Platform/DialogBox.hpp"

#include "YTE/Utilities/String/String.h"
#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  std::wstring cWorkingDirectory = std::experimental::filesystem::current_path();

  // Adapted from http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
  std::vector<std::string> split(const std::string &aString, char aDelimiter, bool aIgnoreEmpty)
  {
    std::stringstream ss(aString);
    std::string item;
    std::vector<std::string> tokens;

    while (std::getline(ss, item, aDelimiter))
    {
      if (aIgnoreEmpty && 0 == item.size())
      {
        continue;
      }

      tokens.push_back(item);
    }

    return tokens;
  }


  filesystem::path relativeTo(filesystem::path from, filesystem::path to)
  {
    // Start at the root path and while they are the same then do nothing then when they first
    // diverge take the remainder of the two path and replace the entire from path with ".."
    // segments.
    filesystem::path::const_iterator fromIter = from.begin();
    filesystem::path::const_iterator toIter = to.begin();

    // Loop through both
    while (fromIter != from.end() && toIter != to.end() && (*toIter) == (*fromIter))
    {
      ++toIter;
      ++fromIter;
    }

    filesystem::path finalPath;
    while (fromIter != from.end())
    {
      finalPath /= "..";
      ++fromIter;
    }

    while (toIter != to.end())
    {
      finalPath /= *toIter;
      ++toIter;
    }

    return finalPath;
  }

  #ifdef Windows
    static int vasprintf(char **strPtr, const char *aFormatString, va_list argv)
    {
      int needed = vsnprintf((*strPtr = nullptr), 0, aFormatString, argv);
  
      if ((needed < 0) || ((*strPtr = (char *)malloc(1 + needed)) == nullptr))
      {
        return -1;
      }
  
      return vsprintf_s(*strPtr, 1 + needed, aFormatString, argv);
    }
  #endif

  std::string Format(const char *aFormatString, va_list aVariadicArguments)
  {
    char *stringBuffer;
    vasprintf(&stringBuffer, aFormatString, aVariadicArguments);

    std::string toReturn(stringBuffer);
    free(stringBuffer);
    return toReturn;
  }

  std::string Format(const char *aFormatString, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, aFormatString);

    return Format(aFormatString, variadicArguments);
  }

  /******************************************************************************/
  /*!
  \brief
  As the name.
  Based on a StackOverflow:
  http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring

  \param name
  Name of the file to read.

  \return
  Success or failure.
  */
  /******************************************************************************/
  bool ReadFileToString(std::string &file, std::string &output)
  {
    std::ifstream stream(file);

    if (!stream.fail())
    {
      stream.seekg(0, std::ios::end);
      output.reserve(static_cast<unsigned int>(stream.tellg()));
      stream.seekg(0, std::ios::beg);

      output.assign((std::istreambuf_iterator<char>(stream)),
	      std::istreambuf_iterator<char>());
      return true;
    }

    return false;
  }

  void StringToFloats(std::string &file, std::vector<float> &output)
  {
    auto strPointer = file.c_str();
  
    while (*strPointer != NULL)
    {
      if (*strPointer != ',')
      {
        output.push_back(static_cast<float>(atof(strPointer)));

        while (*strPointer != NULL && *strPointer != ',')
        {
          ++strPointer;
        }
        if (*strPointer == NULL)
        {
          output.pop_back();
          return;
        }
      }

      ++strPointer;
    }

    output.pop_back();
  }

  std::wstring GetFileOfType(const wchar_t *aFileType, const wchar_t *aDirectory, 
                             const char *aFile, const wchar_t *aFileExtension)
  {
    std::string convert(aFile);
    std::wstring toReturn = L"../";

    toReturn = toReturn + aDirectory + L"/" +
                std::wstring(convert.begin(), convert.end()) +
                aFileExtension;

    toReturn = std::experimental::filesystem::canonical(toReturn, cWorkingDirectory);

    DebugObjection(std::experimental::filesystem::exists(toReturn) == false, 
                "%S of with name of \"%s\" doesn't exist", aFileType, aFile);

    return toReturn;
  }


  std::string GetConfigPath(const String &name)
  {
    auto wpath = GetFileOfType(L"Config", L"Bin", name.c_str(), L".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }


  std::string GetConfigPath(const char *name)
  {
    auto wpath = GetFileOfType(L"Config", L"Bin", name, L".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }


  std::string GetConfigPath(const std::string &name)
  {
    auto wpath = GetFileOfType(L"Config", L"Bin", name.c_str(), L".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }
    
  /////////////////////////////////////////////////
  // Jeffery Uong
  /////////////////////////////////////////////////
  std::string RemoveExtension(const std::string & filename)
  {
    size_t lastdot = filename.find_last_of(".");

    if (lastdot == std::string::npos)
    {
      return filename;
    }

    return filename.substr(0, lastdot);
  }
}