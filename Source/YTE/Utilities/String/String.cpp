#include "YTE/Utilities/String/String.h"

namespace YTE
{
  String::StringNode String::cEmptyNode = { 1, 0, 0, {0} }; 

  bool operator <(const char *lfs, const YTE::String &rhs)
  {
    auto comparison = std::strcmp(lfs, rhs.c_str());
    return comparison < 0;
  }

  bool operator <(const std::string &lfs, const YTE::String &rhs)
  {
    auto comparison = std::strcmp(lfs.c_str(), rhs.c_str());
    return comparison < 0;
  }
}