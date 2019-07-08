#pragma once

#ifndef YTE_WWise_Utilities_hpp
#define YTE_WWise_Utilities_hpp

#include "AK/SoundEngine/Common/AkTypes.h"

namespace YTE
{
  inline AkVector MakeAkVec(glm::vec3 aVector)
  {
    return AkVector{ aVector.x, aVector.y, aVector.z };
  }
}

#endif
