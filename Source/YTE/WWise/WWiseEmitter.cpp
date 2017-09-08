/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-09-19
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/

#include "AK/SoundEngine/Common/AkSoundEngine.h"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/Orientation.h"
#include "YTE/Physics/Transform.h"

#include "YTE/WWise//WWiseSystem.hpp"
#include "YTE/WWise//WWiseEmitter.hpp"
#include "YTE/WWise//Utilities.hpp"

namespace YTE
{
  DefineType(WWiseEmitter)
  {
    YTERegisterType(WWiseEmitter);

    YTEBindField(&WWiseEmitter::mSound, "Sound", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();

    YTEAddFunction( &WWiseEmitter::Play, YTENoOverload, "Play", YTENoNames);
    YTEAddFunction( &WWiseEmitter::PlayEvent, YTENoOverload, "PlaySound", YTEParameterNames("aSound"));
  }

  WWiseEmitter::WWiseEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    AK::SoundEngine::RegisterGameObj(OwnerId(), mOwner->GetName().c_str());

    DeserializeByType<WWiseEmitter*>(aProperties, this, WWiseEmitter::GetStaticType());
  }

  WWiseEmitter::~WWiseEmitter()
  {
    AK::SoundEngine::UnregisterGameObj(OwnerId());
  }

  void WWiseEmitter::Play()
  {
    mSpace->GetEngine()->GetComponent<WWiseSystem>()->SendEvent(mSound, OwnerId());
  }

  void WWiseEmitter::PlayEvent(String aEvent)
  {
    mSpace->GetEngine()->GetComponent<WWiseSystem>()->SendEvent(aEvent, OwnerId());
  }

  void WWiseEmitter::Initialize()
  {
    auto transform = mOwner->GetComponent<Transform>();

    if (transform != nullptr)
    {
      mOwner->CONNECT(Events::PositionChanged, *this, &WWiseEmitter::OnPositionChange);
      mOwner->CONNECT(Events::OrientationChanged, *this, &WWiseEmitter::OnOrientationChange);
      mEmitterPosition.SetPosition(MakeAkVec(transform->GetTranslation()));

      auto orientation = mOwner->GetComponent<Orientation>();

      if (orientation != nullptr)
      {
        mEmitterPosition.SetOrientation(MakeAkVec(orientation->GetForwardVector()),
                                        MakeAkVec(orientation->GetUpVector()));
      }
    }

    SetEmitterPosition();
  }

  void WWiseEmitter::OnPositionChange(const PositionChanged *aEvent)
  {
    mEmitterPosition.SetPosition(MakeAkVec(aEvent->Position));
    SetEmitterPosition();
  }

  void WWiseEmitter::OnOrientationChange(const OrientationChanged *aEvent)
  {
    mEmitterPosition.SetOrientation(MakeAkVec(aEvent->Forward),
                                    MakeAkVec(aEvent->Up));

    SetEmitterPosition();
  }

  void WWiseEmitter::SetEmitterPosition()
  {
    AK::SoundEngine::SetPosition(OwnerId(), mEmitterPosition);
  }
}
