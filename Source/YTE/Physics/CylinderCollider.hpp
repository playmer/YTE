/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2016-03-06
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_CylinderCollder_h
#define YTE_Physics_CylinderCollder_h

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/Collider.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class CylinderCollider : public Collider
  {
  public:
    YTEDeclareType(CylinderCollider);

    YTE_Shared CylinderCollider(Composition *aOwner, Space *aSpace);

    YTE_Shared void PhysicsInitialize() override;

    YTE_Shared void ScaleUpdate(TransformChanged *aEvent);

  private:
    UniquePointer<btCylinderShape> mCylinderShape;
  };
}

#endif
