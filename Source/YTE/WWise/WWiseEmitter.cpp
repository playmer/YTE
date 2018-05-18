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

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseListener.hpp"
#include "YTE/WWise/WWiseSystem.hpp"
#include "YTE/WWise/Utilities.hpp"

namespace YTE
{
  YTEDefineType(WWiseEmitter)
  {
    RegisterType<WWiseEmitter>();
    TypeBuilder<WWiseEmitter> builder;
    

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() }, 
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Field<&WWiseEmitter::mSound>("Sound", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Function<&WWiseEmitter::Play>("Play");
    builder.Function<SelectOverload<void (WWiseEmitter::*)(const std::string&),&WWiseEmitter::PlayEvent>()>("PlaySound")
      .SetParameterNames("aSound");
    builder.Function<SelectOverload<void (WWiseEmitter::*)(u64),&WWiseEmitter::PlayEvent>()>("PlaySound")
      .SetParameterNames("aSound");
  }

  WWiseEmitter::WWiseEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    AK::SoundEngine::RegisterGameObj(OwnerId(), mOwner->GetName().c_str());

    auto view = mSpace->GetComponent<WWiseView>();

    if (view)
    {
      view->RegisterEvent<&WWiseEmitter::ListenerChanged>(Events::WWiseListenerChanged, this);
    }
    else
    {
      std::cout << "No WWiseView on the current space, playing will fail!\n";
    }

    DeserializeByType(aProperties, this, GetStaticType());
  }

  WWiseEmitter::~WWiseEmitter()
  {
    AK::SoundEngine::UnregisterGameObj(OwnerId());
  }

  void WWiseEmitter::Play()
  {
    mSpace->GetEngine()->GetComponent<WWiseSystem>()->SendEvent(mSound, OwnerId());
  }

  void WWiseEmitter::PlayEvent(const std::string &aEvent)
  {
    mSpace->GetEngine()->GetComponent<WWiseSystem>()->SendEvent(aEvent, OwnerId());
  }

  void WWiseEmitter::PlayEvent(u64 aEvent)
  {
    mSpace->GetEngine()->GetComponent<WWiseSystem>()->SendEvent(aEvent, OwnerId());
  }

  void WWiseEmitter::Initialize()
  {
    auto view = mSpace->GetComponent<WWiseView>();

    if (view)
    {
      auto listener = view->GetActiveListener()->OwnerId();
      AK::SoundEngine::SetListeners(OwnerId(), &listener, 1);
    }
    else
    {
      std::cout << "No WWiseView on the current space, playing will fail!\n";
    }

    auto transform = mOwner->GetComponent<Transform>();

    if (transform != nullptr)
    {
      mOwner->RegisterEvent<&WWiseEmitter::OnPositionChange>(Events::PositionChanged, this);
      mOwner->RegisterEvent<&WWiseEmitter::OnOrientationChange>(Events::OrientationChanged, this);
      mEmitterPosition.SetPosition(MakeAkVec(transform->GetTranslation()));

      auto orientation = mOwner->GetComponent<Orientation>();

      mEmitterPosition.SetOrientation(MakeAkVec(orientation->GetForwardVector()),
                                      MakeAkVec(orientation->GetUpVector()));
    }

    SetEmitterPosition();
  }

  void WWiseEmitter::ListenerChanged(WWiseListenerChanged *aListenerChange)
  {
    auto listener = aListenerChange->NewListener->OwnerId();
    AK::SoundEngine::SetListeners(OwnerId(), &listener, 1);
  }

  void WWiseEmitter::OnPositionChange(const TransformChanged *aEvent)
  {
    //std::cout << "Emitter Orientation changed\n";
    mEmitterPosition.SetPosition(MakeAkVec(aEvent->Position));
    SetEmitterPosition();
  }

  void WWiseEmitter::OnOrientationChange(const OrientationChanged *aEvent)
  {
    //std::cout << "Emitter Orientation changed\n";
    mEmitterPosition.SetOrientation(MakeAkVec(aEvent->ForwardVector),
                                    MakeAkVec(aEvent->UpVector));

    SetEmitterPosition();
  }

  void WWiseEmitter::SetEmitterPosition()
  {
    AK::SoundEngine::SetPosition(OwnerId(), mEmitterPosition);
  }
}
