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

#include "YTE/Physics/BoxCollider.h"
#include "YTE/Physics/CapsuleCollider.h"
#include "YTE/Physics/CylinderCollider.h"
#include "YTE/Physics/Collider.h"
#include "YTE/Physics/SphereCollider.h"
#include "YTE/Physics/MeshCollider.h"
#include "YTE/Physics/Transform.h"
#include "PhysicsSystem.h"

namespace YTE
{
  DefineEvent(CollisionStarted);
  DefineEvent(CollisionPersisted);
  DefineEvent(CollisionEnded);

  DefineType(CollisionEvent)
  {
    YTERegisterType(CollisionEvent);
    YTEBindField(&CollisionEvent::OtherObject, "OtherObject", PropertyBinding::Get);
  }


  DefineType(CollisionStarted)
  {
    YTERegisterType(CollisionStarted);
    YTEBindField(&CollisionStarted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionStarted::Object, "Object", PropertyBinding::Get);
  }

  DefineType(CollisionPersisted)
  {
    YTERegisterType(CollisionPersisted);
    YTEBindField(&CollisionPersisted::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionPersisted::Object, "Object", PropertyBinding::Get);
  }

  DefineType(CollisionEnded)
  {
    YTERegisterType(CollisionEnded);
    YTEBindField(&CollisionEnded::Name, "Name", PropertyBinding::Get);
    YTEBindField(&CollisionEnded::Object, "Object", PropertyBinding::Get);
  }

  DefineType(Collider)
  {
    YTERegisterType(Collider);
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
      
    if ((collider = aOwner->GetComponent<BoxCollider>()) != nullptr) return collider;
    if ((collider = aOwner->GetComponent<CylinderCollider>()) != nullptr) return collider;
    if ((collider = aOwner->GetComponent<SphereCollider>()) != nullptr) return collider;
    if ((collider = aOwner->GetComponent<CapsuleCollider>()) != nullptr) return collider;
    if ((collider = aOwner->GetComponent<MeshCollider>()) != nullptr) return collider;

    return collider;
  }
}
