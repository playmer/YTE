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



#include "YTE/Physics/Collider.h"

namespace YTE
{
  class CylinderCollider : public Collider
  {
  public:
    DeclareType(CylinderCollider);

    CylinderCollider(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void PhysicsInitialize();

  private:
    UniquePointer<btCylinderShape> mCylinderShape;
  };
}

#endif
