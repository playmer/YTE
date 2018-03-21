/******************************************************************************/
/*!
\file   CameraController.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27
\brief
This component controls the logic for the camera. Eventually this will handle
input from the controller as well as cinematic moves when starting dialogue
etc.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Physics/Orientation.hpp"

#include "YTE/Graphics/FlybyCamera.hpp"

#include "YTE/GameComponents/CameraAnchor.hpp"

namespace YTE
{
  class CameraController : public Component
  {
  public:
    YTEDeclareType(CameraController);
    CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnAttachCamera(AttachCamera *aEvent);
    void OnAnchorPositionUpdate(TransformChanged* aEvent);
    void OnAnchorRotationUpdate(TransformChanged* aEvent);

    void OnDebugSwitch(DebugSwitch *aEvent);

    /*void RotateCamera(CameraRotateEvent *aEvent);
    void OnDirectCamera(DirectCameraEvent *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);*/

  private:
    Transform *mBoatTransform;
    Orientation *mBoatOrientation;
    float mRotationAngle;
    Transform *mTransform;
    Orientation *mOrientation;
    Camera *mCameraComponent;

    Composition *mAnchor;
    Transform *mAnchorTransform;

    FlybyCamera *mFlybyComponent;

    //void RotateOnBoom(const glm::vec3& aFocusPoint);
  };
}
