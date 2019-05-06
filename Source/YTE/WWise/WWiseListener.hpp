#pragma once

#ifndef YTE_WWise_WWiseListener_h
#define YTE_WWise_WWiseListener_h

#include "AK/SoundEngine/Common/AkTypes.h"

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class WWiseListener : public Component
  {
  public:
    YTEDeclareType(WWiseListener);

    YTE_Shared WWiseListener(Composition *aOwner, Space *aSpace);

    YTE_Shared ~WWiseListener() override;

    YTE_Shared void Initialize() override;

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

  private:
    void SetListenerPosition();
    void OnPositionChange(const TransformChanged *aEvent);
    void OnOrientationChange(const OrientationChanged *aEvent);

    AkListenerPosition mListenerPosition;
  };
}

#endif
