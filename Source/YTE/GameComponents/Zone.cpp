#include "YTE/GameComponents/Zone.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{

  YTEDefineType(Zone)
  {
    RegisterType<Zone>();
    TypeBuilder<Zone> builder;

    builder.Property<&Zone::GetZoneName, &Zone::SetZoneName>( "ZoneName")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&Zone::GetZoneType, &Zone::SetZoneType>( "ZoneType")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  Zone::Zone(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void Zone::Initialize()
  {
    mZoneCollider = mOwner->GetComponent<CylinderCollider>();
    mZoneBody = mOwner->GetComponent<GhostBody>();
  }

}