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

namespace YTE
{
  YTEDeclareEvent(OrientationChanged);

  class OrientationChanged : public Event
  {
  public:
    YTEDeclareType(OrientationChanged);

    glm::vec3 Forward;
    glm::vec3 Right;
    glm::vec3 Up;
  };

  class Orientation : public Component
  {
  public:
    YTEDeclareType(Orientation);

    Orientation(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void OnRotationChanged(RotationChanged *aEvent);

    const glm::vec3& GetForwardVector() const;
    const glm::vec3& GetRightVector() const;
    const glm::vec3& GetUpVector() const;

    void LookAtPoint(glm::vec3 &aPoint);
    void LookAtPointWithUp(glm::vec3 &aPoint, glm::vec3 &aUp);
    void LookAtDirection(glm::vec3 &aDirection);
    void LookAtDirectionWithUp(glm::vec3 &aDirection, glm::vec3 &aUp);

  private:
    glm::vec3 mForwardVector;
    glm::vec3 mRightVector;
    glm::vec3 mUpVector;
  };
}

#endif
