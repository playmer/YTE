#pragma once

#include <string>
#include <vector>

#include "YTE/Platform/TargetDefinitions.hpp"

struct ApplicationArguments
{
  ApplicationArguments(int aArgumentsSize = 0, char** aArguments = nullptr)
    : mOriginalArgumentsSize{ aArgumentsSize }
    , mOriginalArguments{ aArguments }
  {
    mArguments.reserve(aArgumentsSize);

    for (int i = 0; i < aArgumentsSize; ++i)
    {
      mArguments.emplace_back(aArguments[i]);
    }
  }

  std::vector<std::string> mArguments;
  int mOriginalArgumentsSize;
  char** mOriginalArguments;
};

extern int YTE_Main(ApplicationArguments& aArguments);