/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_WWise_WWiseListener_h
#define YTE_WWise_WWiseListener_h

#include "YTE/Core/Component.hpp"
#include "AK/SoundEngine/Common/AkTypes.h"

namespace YTE
{
  class WWiseListener : public Component
  {
  public:
    YTEDeclareType(WWiseListener);

    WWiseListener(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void Initialize() override;

    ~WWiseListener() override;

  private:
    void SetListenerPosition();
    void OnPositionChange(const PositionChanged *aEvent);
    void OnOrientationChange(const OrientationChanged *aEvent);

    AkListenerPosition mListenerPosition;
  };
}

#endif
