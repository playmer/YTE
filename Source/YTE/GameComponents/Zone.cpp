#include "YTE/GameComponents/Zone.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{

  YTEDefineType(Zone)
  {
    YTERegisterType(Zone);

    YTEBindProperty(&Zone::GetZoneName, &Zone::SetZoneName, "Zone Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  Zone::Zone(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void Zone::Initialize()
  {
    mZoneCollider = mOwner->GetComponent<BoxCollider>();
    mZoneBody = mOwner->GetComponent<GhostBody>();
  }

}