/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-28
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_Orientation_hpp
#define YTE_Utilities_Orientation_hpp

#include "YTE/Core/Component.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDeclareEvent(OrientationChanged);

  class OrientationChanged : public Event
  {
  public:
    YTEDeclareType(OrientationChanged);

    Orientation *Orientation;
    glm::vec3 ForwardVector;
    glm::vec3 RightVector;
    glm::vec3 UpVector;
  };

  class Orientation : public Component
  {
  public:
    YTEDeclareType(Orientation);

    YTE_Shared Orientation(Composition *aOwner, Space *aSpace);

    YTE_Shared void Initialize() override;
    YTE_Shared void OnRotationChanged(TransformChanged *aEvent);

    YTE_Shared glm::vec3 GetForwardVector() const;
    YTE_Shared glm::vec3 GetRightVector() const;
    YTE_Shared glm::vec3 GetUpVector() const;

    YTE_Shared void LookAt(glm::vec3 const &aDirection);
    YTE_Shared void LookAtPoint(glm::vec3 const &aPoint);

  private:
    glm::vec3 mForwardVector;
    glm::vec3 mRightVector;
    glm::vec3 mUpVector;
    Transform *mTransform;
  };
}

#endif