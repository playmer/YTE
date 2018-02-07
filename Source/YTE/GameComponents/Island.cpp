#include "YTE/GameComponents/Island.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{

  YTEDefineType(Island)
  {
    YTERegisterType(Island);
  }

  Island::Island(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void Island::Initialize()
  {
    mIslandBody = mOwner->GetComponent<GhostBody>();
    mIslandCollider = mOwner->GetComponent<BoxCollider>();
  }

}