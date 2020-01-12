#pragma once

#include "YTE/Graphics/Animation.hpp"

#include "YTETools/YTEToolsMeta.hpp"

namespace YTE::Tools 
{
  YTETools_Shared std::vector<char> WriteAnimationDataToFile(std::string const& aName, AnimationData const& aData);
  YTETools_Shared AnimationData ImportAnimation(std::string const& aName);
}