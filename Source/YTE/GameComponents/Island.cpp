#include "YTE/GameComponents/Island.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{

  YTEDefineType(Island)
  {
    RegisterType<Island>();
    TypeBuilder<Island> builder;
  }

  Island::Island(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    UnusedArguments(aProperties);
  }

  void Island::Initialize()
  {
    mIslandBody = mOwner->GetComponent<GhostBody>();
    mIslandCollider = mOwner->GetComponent<BoxCollider>();
  }

}