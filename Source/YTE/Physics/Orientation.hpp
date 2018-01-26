/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-28
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_Orientation_h
#define YTE_Utilities_Orientation_h

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
    glm::quat Forward;
    glm::quat Right;
    glm::quat Up;
  };

  class Orientation : public Component
  {
  public:
    YTEDeclareType(Orientation);

    Orientation(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void OnRotationChanged(TransformChanged *aEvent);

    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetUpVector() const;

    glm::quat GetForward() const;
    glm::quat GetRight() const;
    glm::quat GetUp() const;

  private:
    glm::quat mForward;
    glm::quat mRight;
    glm::quat mUp;
    glm::vec3 mForwardVector;
    glm::vec3 mRightVector;
    glm::vec3 mUpVector;
  };
}

#endif