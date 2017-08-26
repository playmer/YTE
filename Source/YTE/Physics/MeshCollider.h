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

#include "YTE/Event/StandardEvents.h"

#include "YTE/Physics/Collider.h"

namespace YTE
{
  class MeshCollider : public Collider
  {
  public:
    DeclareType(MeshCollider);

    MeshCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void PhysicsInitialize();
  private:
    UniquePointer<btBvhTriangleMeshShape> mTriangleMeshShape;
    btTriangleMesh mTriangles;
  };
}

#endif
