/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_WWise_WWiseEmitter_h
#define YTE_WWise_WWiseEmitter_h

#include "AK/SoundEngine/Common/AkTypes.h"

#include "YTE/Core/Component.hpp"



namespace YTE
{
  class WWiseEmitter : public Component
  {
  public:
    YTEDeclareType(WWiseEmitter);

    WWiseEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~WWiseEmitter() override;

    void Play();
    void PlayEvent(const std::string & aEvent);

    void Initialize() override;


    inline AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(mOwner); };

  private:
    void SetEmitterPosition();
    void OnPositionChange(const TransformChanged *aEvent);
    void OnOrientationChange(const OrientationChanged *aEvent);

    AkSoundPosition mEmitterPosition;
    int mListenerId;

    std::string mSound;
    //bool mPlaying;
    float mVolume;
  };
}

#endif
