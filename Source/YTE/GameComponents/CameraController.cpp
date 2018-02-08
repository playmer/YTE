/******************************************************************************/
/*!
\file   CameraController.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/CameraController.hpp"

namespace YTE
{
  YTEDefineType(CameraController)
  {
    YTERegisterType(CameraController);
  }

  CameraController::CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mTransform(aOwner->GetComponent<Transform>())
    , mOrientation(aOwner->GetComponent<Orientation>())
    , mRotationAngle(0.0f)
    , mBoatTransform(aOwner->GetParent()->GetComponent<Transform>())
    , mBoatOrientation(aOwner->GetParent()->GetComponent<Orientation>())
  {
    YTEUnusedArgument(aProperties);
    //will probably have some props
  }

  void CameraController::Initialize()
  {
    mOwner->GetEngine()->YTERegister(Events::CameraRotateEvent, this, &CameraController::RotateCamera);
  }

  void CameraController::RotateCamera(CameraRotateEvent *aEvent)
  {
    mRotationAngle -= aEvent->StickDirection.x / 2.0f;
    if (mRotationAngle > 360.0f)
    {
      mRotationAngle = 0.0f;
    }
    if (mRotationAngle < 0.0f)
    {
      mRotationAngle = 360.0f;
    }
    glm::vec3 camAxisAngle = glm::vec3(0, mRotationAngle, 0);

    glm::quat rot = glm::angleAxis(mRotationAngle, mOrientation->GetUpVector());
    glm::vec3 newRot = glm::rotate(rot, mOrientation->GetForwardVector());
    //mTransform->SetRotationProperty(newRot);
    mTransform->SetWorldRotationProperty(newRot);

    /*
    glm::vec3 camForwardPoint = mTransform->GetTranslation() + camForward;
    glm::vec3 translationVec = mBoatTransform->GetTranslation() - camForwardPoint;
    //dont add the y component
    translationVec.y = 0.0f;
    
    mTransform->SetTranslation(mTransform->GetTranslation() + translationVec);
    mTransform->SetRotationProperty(camForward);
    */
  }
}// end yte namespace