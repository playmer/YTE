#pragma once

namespace YTE::Tools
{
  void WriteSkeletonToFile(std::string const& aName, Skeleton const& aSkeleton);
  void ImportSkelton(std::string const& aName, Skeleton& aSkeleton);
}