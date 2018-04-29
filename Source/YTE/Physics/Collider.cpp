/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/CapsuleCollider.hpp"
#include "YTE/Physics/CylinderCollider.hpp"
#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/SphereCollider.hpp"
#include "YTE/Physics/MeshCollider.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"

namespace YTE
{
  YTEDefineEvent(CollisionStarted);
  YTEDefineEvent(CollisionPersisted);
  YTEDefineEvent(CollisionEnded);

  YTEDefineType(CollisionEvent)
  {
    RegisterType<CollisionEvent>();
    YTEBindField(&CollisionEvent::OtherObject, "OtherObject", PropertyBinding::Get);
  }


  YTEDefineType(CollisionStarted)
  {
    RegisterType<CollisionStarted>();
    YTEBindField(&CollisionStarted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionStarted::Object, "Object", PropertyBinding::Get);
  }

  YTEDefineType(CollisionPersisted)
  {
    RegisterType<CollisionPersisted>();
    YTEBindField(&CollisionPersisted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionPersisted::Object, "Object", PropertyBinding::Get);
  }

  YTEDefineType(CollisionEnded)
  {
    RegisterType<CollisionEnded>();
    YTEBindField(&CollisionEnded::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionEnded::Object, "Object", PropertyBinding::Get);
  }

  YTEDefineType(Collider)
  {
    RegisterType<Collider>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  Collider::Collider(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
  {
  }

  glm::vec3 Collider::GetTranslation() const
  {
    auto bulletTranslation = mCollider->getWorldTransform().getOrigin();

    return glm::vec3(bulletTranslation.x(), bulletTranslation.y(), bulletTranslation.z());
  }

  glm::quat Collider::GetRotation() const
  {
    auto bulletRotation = mCollider->getWorldTransform().getRotation();

    return glm::quat(bulletRotation.w(), bulletRotation.x(), bulletRotation.y(), bulletRotation.z());
  }

  void Collider::SetTranslation(float aX, float aY, float aZ)
  {
    auto translation = btVector3(aX, aY, aZ);

    mCollider->getWorldTransform().setOrigin(translation);
  }

  void Collider::SetRotation(const glm::quat& aRotation)
  {
    auto rotation = OurQuatToBt(aRotation);

    mCollider->getWorldTransform().setRotation(rotation);
  }

  Collider* GetColliderFromObject(Composition* aOwner)
  {
    Collider* collider = nullptr;
      
    if (nullptr != (collider = aOwner->GetComponent<BoxCollider>())) return collider;
    if (nullptr != (collider = aOwner->GetComponent<CylinderCollider>())) return collider;
    if (nullptr != (collider = aOwner->GetComponent<SphereCollider>())) return collider;
    if (nullptr != (collider = aOwner->GetComponent<CapsuleCollider>())) return collider;
    if (nullptr != (collider = aOwner->GetComponent<MeshCollider>())) return collider;

    return collider;
  }
}
