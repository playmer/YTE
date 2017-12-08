/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_MeshCollider_h
#define YTE_Physics_MeshCollider_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"

namespace YTE
{
  class MeshCollider : public Collider
  {
  public:
    YTEDeclareType(MeshCollider);

    MeshCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void PhysicsInitialize() override;

  private:
    UniquePointer<btBvhTriangleMeshShape> mTriangleMeshShape;
    btTriangleMesh mTriangles;
  };
}

#endif
