#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <memory>
#include <random>
#include <cstdlib>
#include "YTE/StandardLibrary/FileSystem.hpp"
#include <stdarg.h> /* va_list, va_start, va_end*/
#include <stdio.h>
#include <random>

#include "YTE/Platform/DialogBox.hpp"

#include "YTE/Utilities/String/String.hpp"
#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  std::wstring cWorkingDirectory = std::filesystem::current_path().wstring();

  template <typename tType, typename tRandomEngine>
  tType randomInteger(tRandomEngine &e)
  {
    std::uniform_int_distribution<tType> dist{ std::numeric_limits<tType>::min(), std::numeric_limits<tType>::max() };

    return dist(e);
  }

  GlobalUniqueIdentifier::GlobalUniqueIdentifier()
  {
    std::random_device rd;
    std::mt19937 gen(rd());

    mPart1 = randomInteger<decltype(mPart1)>(gen);
    mPart2 = randomInteger<decltype(mPart2)>(gen);
    mVersion = randomInteger<decltype(mVersion)>(gen);
    mVariant = randomInteger<decltype(mVariant)>(gen);
    mPart3 = randomInteger<decltype(mPart3)>(gen);
    mPart4 = randomInteger<decltype(mPart4)>(gen);

    // Version
    u16 version4Flags = 0b0100000000000000;
    u16 version4Mask = 0b0000111111111111;
    u16 version4AffectedFlags = (version4Mask ^ -1LL);

    mVersion = (mVersion & version4Mask) | (version4Flags & version4AffectedFlags);

    // Variant
    u16 variant1Flags = 0b1000000000000000;
    u16 variant1Mask = 0b0011111111111111;
    u16 variant1AffectedFlags = (variant1Mask ^ -1LL);

    mVariant = (mVariant & variant1Mask) | (variant1Flags & variant1AffectedFlags);
  }

  std::string GlobalUniqueIdentifier::ToString() const
  {
    std::string total{ '{' };
    total += Format("%08X", mPart1);
    total += '-';
    total += Format("%04X", mPart2);
    total += '-';
    total += Format("%04X", mVersion);
    total += '-';
    total += Format("%04X", mVariant);
    total += '-';
    total += Format("%08X", mPart3);
    total += Format("%04X", mPart4);
    total += '}';

    return total;
  }

  std::string GlobalUniqueIdentifier::ToIdentifierString() const
  {
    std::string total{ "GUID_" };
    total += Format("%08X", mPart1);
    total += '_';
    total += Format("%04X", mPart2);
    total += '_';
    total += Format("%04X", mVersion);
    total += '_';
    total += Format("%04X", mVariant);
    total += '_';
    total += Format("%08X", mPart3);
    total += Format("%04X", mPart4);

    return total;
  }

  bool GlobalUniqueIdentifier::operator==(GlobalUniqueIdentifier const& aGUID)
  {
    if (this->mPart1 == aGUID.mPart1 &&
        this->mPart2 == aGUID.mPart2 &&
        this->mVersion == aGUID.mVersion &&
        this->mVariant == aGUID.mVariant &&
        this->mPart3 == aGUID.mPart3 &&
        this->mPart4 == aGUID.mPart4)
    {
      return true;
    }

    return false;
  }

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

  #if YTE_Windows
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

  // Based on a StackOverflow:
  // http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  bool ReadFileToString(std::string const &file, std::string &output)
  {
    std::ifstream stream(file);

    if (!stream.fail())
    {
      stream.seekg(0, std::ios::end);
      output.reserve(output.size() +  static_cast<unsigned int>(stream.tellg()));
      stream.seekg(0, std::ios::beg);

      output.insert(output.end(), std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
      return true;
    }

    return false;
  }

  // Based on:
  // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
  std::vector<byte> ReadFileToVector(std::string const& file)
  {
    std::basic_ifstream<byte, std::char_traits<byte>> stream(file);
    std::vector<byte> output;

    if (!stream.fail())
    {
      stream.seekg(0, std::ios::end);
      output.resize(static_cast<size_t>(stream.tellg()));
      stream.seekg(0, std::ios::beg);
      stream.read(output.data(), output.size());
      stream.close();
    }

    return std::move(output);
  }

  void StringToFloats(std::string &file, std::vector<float> &output)
  {
    auto strPointer = file.c_str();
  
    while ('\0' != *strPointer)
    {
      if (*strPointer != ',')
      {
        output.push_back(static_cast<float>(atof(strPointer)));

        while (('\0' != *strPointer) && (*strPointer != ','))
        {
          ++strPointer;
        }
        if ('\0' == *strPointer)
        {
          output.pop_back();
          return;
        }
      }

      ++strPointer;
    }

    output.pop_back();
  }

  std::string GetFileOfType(const char *aFileType, const char *aDirectory, 
                             const char *aFile, const char *aFileExtension)
  {
    std::string convert(aFile);
    std::string toReturn = "../";

    toReturn = toReturn + aDirectory + "/" +
                std::string(convert.begin(), convert.end()) +
                aFileExtension;

    std::filesystem::path basePath{ cWorkingDirectory };
    std::filesystem::path toReturnPath{ toReturn };
    std::filesystem::path baseAndToReturn = basePath / toReturnPath;

    toReturn = std::filesystem::canonical(baseAndToReturn).u8string();

    //DebugObjection(std::filesystem::exists(toReturn) == false, 
    //            "%S of with name of \"%s\" doesn't exist", aFileType, aFile);
    UnusedArguments(aFileType);

    return toReturn;
  }


  std::string GetConfigPath(const String &name)
  {
    auto wpath = GetFileOfType("Config", "Bin", name.c_str(), ".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }


  std::string GetConfigPath(const char *name)
  {
    auto wpath = GetFileOfType("Config", "Bin", name, ".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }


  std::string GetConfigPath(const std::string &name)
  {
    auto wpath = GetFileOfType("Config", "Bin", name.c_str(), ".json");
    std::string str{ wpath.begin(), wpath.end() };

    return str;
  }

  bool FileCheck(const Path& aPath, const std::string& aDirectory, std::string &aFile)
  {
    if (0 == aFile.size())
    {
      return false;
    }

    std::filesystem::path pathName{ aPath.String() };
    pathName.append(aDirectory);
    pathName.append(aFile);
    return std::filesystem::exists(pathName);
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