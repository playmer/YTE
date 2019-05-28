#include "AK/SoundEngine/Common/AkSoundEngine.h"

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/WWise//WWiseView.hpp"
#include "YTE/WWise//WWiseListener.hpp"

namespace YTE
{
  YTEDefineType(WWiseListener)
  {
    RegisterType<WWiseListener>();
    TypeBuilder<WWiseListener> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() }, 
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  WWiseListener::WWiseListener(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
  {
    AK::SoundEngine::RegisterGameObj(OwnerId(), mOwner->GetName().c_str());
    AK::SoundEngine::SetListenerSpatialization(OwnerId(), true, AkChannelConfig{});

    auto view = mSpace->GetComponent<WWiseView>();

    if (view)
    {
      view->SetActiveListener(this);
    }
    else
    {
      std::cout << "No WWiseView on the current space, playing will fail!\n";
    }
  }

  WWiseListener::~WWiseListener()
  {
    AK::SoundEngine::UnregisterGameObj(OwnerId());
  }

  void WWiseListener::Initialize()
  {
    auto transform = mOwner->GetComponent<Transform>();

    mOwner->RegisterEvent<&WWiseListener::OnPositionChange>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&WWiseListener::OnOrientationChange>(Events::OrientationChanged, this);
    mListenerPosition.SetPosition(MakeAkVec(transform->GetTranslation()));

    auto orientation = mOwner->GetComponent<Orientation>();
    mListenerPosition.SetOrientation(MakeAkVec(orientation->GetForwardVector()),
                                     MakeAkVec(orientation->GetUpVector()));

    SetListenerPosition();
  }

  void WWiseListener::OnPositionChange(const TransformChanged *aEvent)
  {
    //std::cout << "Listener Position changed\n";
    mListenerPosition.SetPosition(MakeAkVec(aEvent->Position));

    SetListenerPosition();
  }

  void WWiseListener::OnOrientationChange(const OrientationChanged *aEvent)
  {
    //std::cout << "Listener Orientation changed\n";
    mListenerPosition.SetOrientation(MakeAkVec(aEvent->ForwardVector),
                                     MakeAkVec(aEvent->UpVector));

    SetListenerPosition();
  }

  void WWiseListener::SetListenerPosition()
  {
    AK::SoundEngine::SetPosition(OwnerId(), mListenerPosition);
  }
}
