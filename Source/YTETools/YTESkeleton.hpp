#pragma once

#include "YTETools/YTEToolsMeta.hpp"

namespace YTE::Tools
{
  YTETools_Shared void WriteSkeletonToFile(std::string const& aName, Skeleton const& aSkeleton);
  YTETools_Shared void ImportSkelton(std::string const& aName, Skeleton& aSkeleton);
}