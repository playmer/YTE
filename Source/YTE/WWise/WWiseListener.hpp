#pragma once

#ifndef YTE_WWise_WWiseListener_h
#define YTE_WWise_WWiseListener_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

#include "YTE/WWise/ForwardDeclarations.hpp"

namespace YTE
{
  class WWiseListener : public Component
  {
  public:
    YTEDeclareType(WWiseListener);

    YTE_Shared WWiseListener(Composition *aOwner, Space *aSpace);

    YTE_Shared ~WWiseListener() override;

    YTE_Shared void Initialize() override;

    WwiseObject OwnerId() { return reinterpret_cast<WwiseObject>(this); };

  private:
    void SetListenerPosition();
    void OnPositionChange(const TransformChanged *aEvent);
    void OnOrientationChange(const OrientationChanged *aEvent);

    PrivateImplementationLocal<128> mListenerPosition;
  };
}

#endif
