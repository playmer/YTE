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
    CameraBoom(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // Properties /////////////////////////////////////////////////////////////
    const glm::vec2 GetCameraTurnSpeed() const { return mCamTurnSpeed; }
    void SetCameraTurnSpeed(const glm::vec2& aTurnSpeed) { mCamTurnSpeed = aTurnSpeed; }
    ///////////////////////////////////////////////////////////////////////////

    void OnStart(LogicUpdate *);
    void OnLogicUpdate(LogicUpdate *aEvent);
    void OnParentRotated(TransformChanged *);
    void OnCameraRotate(CameraRotateEvent *aEvent);

  private:
    Transform * mTransform;
    Orientation *mOrientation;

    glm::vec2 mCamTurnSpeed;

    bool mIsTurning;
    float mTurnScale;
    glm::vec2 mTurnDirection;
    glm::quat mLastGoodRotation;
  };
}
