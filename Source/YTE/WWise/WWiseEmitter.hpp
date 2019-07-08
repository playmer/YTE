#pragma once

#ifndef YTE_WWise_WWiseEmitter_h
#define YTE_WWise_WWiseEmitter_h

#include "YTE/Core/Component.hpp"

#include "YTE/StandardLibrary/PrivateImplementation.hpp"

#include "YTE/WWise/WWiseView.hpp"

#include "YTE/WWise/ForwardDeclarations.hpp"

namespace YTE
{
  class WWiseEmitter : public Component
  {
  public:
    YTEDeclareType(WWiseEmitter);

    YTE_Shared WWiseEmitter(Composition *aOwner, Space *aSpace);

    YTE_Shared ~WWiseEmitter() override;

    YTE_Shared void Play();
    YTE_Shared void PlayEvent(const std::string & aEvent);
    YTE_Shared void PlayEvent(u64 aEvent);

    YTE_Shared void ListenerChanged(WWiseListenerChanged *aListenerChange);

    YTE_Shared void Initialize() override;


    inline WwiseObject OwnerId() { return reinterpret_cast<WwiseObject>(mOwner); };

  private:
    void SetEmitterPosition();
    void OnPositionChange(const TransformChanged *aEvent);
    void OnOrientationChange(const OrientationChanged *aEvent);

    PrivateImplementationLocal<128> mEmitterPosition;
    int mListenerId;

    std::string mSound;
    float mVolume;
  };
}

#endif
