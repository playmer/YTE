/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
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
