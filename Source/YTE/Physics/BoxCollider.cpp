/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Physics/CollisionBody.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/Physics/GhostBody.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Physics/RigidBody.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(BoxCollider)
  {
    YTERegisterType(BoxCollider);
    YTEBindProperty(&BoxCollider::GetSize, &BoxCollider::SetSizeProperty, "Size")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&BoxCollider::GetOffset, &BoxCollider::SetOffsetProperty, "Offset")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindFunction(&BoxCollider::SetSize, (void (BoxCollider::*) (const glm::vec3&)), "SetSize", YTEParameterNames("aSize"))
      .SetDocumentation("Sets the collider scale as a multiple of the scale of the transform");
    YTEBindFunction(&BoxCollider::SetSize, (void (BoxCollider::*) (float, float, float)), "SetSize", YTEParameterNames("aX" ,"aY" ,"aZ"))
      .SetDocumentation("Sets the collider scale as a multiple of the scale of the transform");

    YTEBindFunction(&BoxCollider::SetOffset, (void (BoxCollider::*) (const glm::vec3&)), "SetOffset", YTEParameterNames("aOffset"))
      .SetDocumentation("Sets the collider position offset from the World Translation of the transform");
    YTEBindFunction(&BoxCollider::SetOffset, (void (BoxCollider::*) (float, float, float)), "SetOffset", YTEParameterNames("aX", "aY", "aZ"))
      .SetDocumentation("Sets the collider position offset from the World Translation of the transform");
  }

  BoxCollider::BoxCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Collider(aOwner, aSpace), mSize(1.f, 1.f, 1.f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void BoxCollider::PhysicsInitialize()
  {
      // Get info from transform and feed that ish to the Bullet collider
    auto transform = mOwner->GetComponent<Transform>();
    auto translation = transform->GetWorldTranslation();
    auto scale = transform->GetWorldScale() / 2.0f;
    auto rotation = transform->GetWorldRotation();
    auto bulletRot = OurQuatToBt(rotation);
    auto bulletTransform = btTransform(bulletRot,
                                        btVector3(translation.x + mOffset.x, 
                                                  translation.y + mOffset.y, 
                                                  translation.z + mOffset.z));

    btVector3 size{ mSize.x * scale.x, mSize.y * scale.y, mSize.z * scale.z };
    mBoxShape = std::make_unique<btBoxShape>(size);

    mCollider = std::make_unique<btCollisionObject>();
    mCollider->setCollisionShape(mBoxShape.get());
    mCollider->setWorldTransform(bulletTransform);
    mCollider->setUserPointer(mOwner);
  }

  void BoxCollider::SetSize(const glm::vec3& aSize)
  { 
    mSize = aSize;

      // Update the underlying collider if it exists
    if (mBoxShape)
    {
      auto scale = mOwner->GetComponent<Transform>()->GetWorldScale() / 2.0f;
      auto localScaling = btVector3(aSize.x * scale.x, aSize.y * scale.y, aSize.z * scale.z);

      mBoxShape->setLocalScaling(localScaling);
    }
  }

  void BoxCollider::SetSize(float aX, float aY, float aZ)
  {
    auto size = glm::vec3(aX, aY, aZ);
      
    SetSize(size);
  }

  void BoxCollider::SetOffset(const glm::vec3& aOffset)
  {
    mOffset = aOffset;

      // Update the underlying collider if it exists
    if (mCollider)
    {
      auto transform = mOwner->GetComponent<Transform>();
      auto translation = transform->GetWorldTranslation();
      auto rotation = transform->GetWorldRotation();
      auto bulletRot = OurQuatToBt(rotation);
      auto bulletTransform = btTransform(bulletRot,
                                          btVector3(translation.x + mOffset.x, 
                                                    translation.y + mOffset.y, 
                                                    translation.z + mOffset.z));

      mCollider->setWorldTransform(bulletTransform);
    }
  }

  void BoxCollider::SetOffset(float aX, float aY, float aZ)
  {
    auto offset = glm::vec3(aX, aY, aZ);

    SetOffset(offset);
  }
}
