/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "AK/SoundEngine/Common/AkSoundEngine.h"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"



#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/WWise//WWiseListener.hpp"
#include "YTE/WWise//Utilities.hpp"

namespace YTE
{
  YTEDefineType(WWiseListener)
  {
    YTERegisterType(WWiseListener);

  }

  WWiseListener::WWiseListener(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<WWiseListener*>(aProperties, this, WWiseListener::GetStaticType());
  }

  WWiseListener::~WWiseListener()
  {
  }

  void WWiseListener::Initialize()
  {
    auto transform = mOwner->GetComponent<Transform>();

    mOwner->YTERegister(Events::PositionChanged, this, &WWiseListener::OnPositionChange);
    mOwner->YTERegister(Events::OrientationChanged, this, &WWiseListener::OnOrientationChange);
    mListenerPosition.SetPosition(MakeAkVec(transform->GetTranslation()));

    auto orientation = mOwner->GetComponent<Orientation>();
    mListenerPosition.SetOrientation(MakeAkVec(orientation->GetForwardVector()),
                                     MakeAkVec(orientation->GetUpVector()));

    SetListenerPosition();
  }

  void WWiseListener::OnPositionChange(const PositionChanged *aEvent)
  {
    mListenerPosition.SetPosition(MakeAkVec(aEvent->Position));

    SetListenerPosition();
  }

  void WWiseListener::OnOrientationChange(const OrientationChanged *aEvent)
  {
    mListenerPosition.SetOrientation(MakeAkVec(aEvent->Forward),
                                      MakeAkVec(aEvent->Up));

    SetListenerPosition();
  }

  void WWiseListener::SetListenerPosition()
  {
    AK::SoundEngine::SetPosition(OwnerId(), mListenerPosition);
  }
}
