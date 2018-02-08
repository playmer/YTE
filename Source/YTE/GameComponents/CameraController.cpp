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
#include "YTE/Graphics/Camera.hpp"

namespace YTE
{
  YTEDefineType(CameraController)
  {
    YTERegisterType(CameraController);
  }

  CameraController::CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mTransform(nullptr)
    , mOrientation(nullptr)
    , mRotationAngle(0.0f)
    , mBoatTransform(nullptr)
    , mBoatOrientation(nullptr)
    , mCameraComponent(nullptr)
  {
    YTEUnusedArgument(aProperties);
    //will probably have some props
  }

  void CameraController::Initialize()
  {
    mOwner->GetEngine()->YTERegister(Events::CameraRotateEvent, this, &CameraController::RotateCamera);

    mTransform = mOwner->GetComponent<Transform>();
    mOrientation = mOwner->GetComponent<Orientation>();
    mCameraComponent = mOwner->GetComponent<Camera>();

    Composition *boat = mOwner->GetParent();
    mBoatTransform = boat->GetComponent<Transform>();
    mBoatOrientation = boat->GetComponent<Orientation>();
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
    /*
    glm::vec3 camAxisAngle = glm::vec3(0, mRotationAngle, 0);

    glm::quat rot = glm::angleAxis(mRotationAngle, mOrientation->GetUpVector());
    glm::vec3 newRot = glm::rotate(rot, mOrientation->GetForwardVector());
    //mTransform->SetRotationProperty(newRot);
    mTransform->SetWorldRotationProperty(newRot);
    */
    //UBOView view;
    //view.mViewMatrix = glm::lookAt(mTransform->GetWorldTranslation(), mBoatTransform->GetWorldTranslation(), glm::vec3(0, 1, 0));
    //view.mCameraPosition = glm::vec4(mTransform->GetWorldTranslation(), 1.0f);
    //
    //mCameraComponent->SetUBOView(view);

    mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() + glm::vec3(0.0f, 3.0f, 0.0f));
    mTransform->SetWorldRotationProperty(glm::vec3(0.0f, mRotationAngle, 0.0f));
    glm::quat rot = mTransform->GetWorldRotation();
    glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 1.0f); // used to translate away from target point by 1
    unitVector = glm::rotate(rot, unitVector);
    unitVector = glm::normalize(unitVector);
    unitVector = 15.0f * unitVector;
    mTransform->SetWorldTranslation(mTransform->GetWorldTranslation() - glm::vec3(unitVector));
    mCameraComponent->SetTargetPoint(mBoatTransform->GetWorldTranslation());

    //mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() - mOrientation->GetForwardVector() * 10.0f);

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