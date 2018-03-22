/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/22
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/Orientation.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
  class CameraBoom : public Component
  {
  public:
    YTEDeclareType(CameraBoom);
    CameraBoom(Composition *aOwner, Space *aSpace, RSValue *);

    void Initialize() override;

    void OnLogicUpdate(LogicUpdate *aEvent);
    void OnCameraRotate(CameraRotateEvent *aEvent);

  private:
    Transform * mTransform;
    Orientation *mOrientation;

    float mRotScale;
    glm::vec2 mRotDirection;
  };
}
