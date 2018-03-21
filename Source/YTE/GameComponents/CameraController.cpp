/******************************************************************************/
/*!
\file   CameraController.cpp
\author Jonathan Ackerman
        Isaac Dayton
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

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  CameraController::CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mTransform(nullptr)
    , mOrientation(nullptr)
    , mRotationAngle(0.0f)
    , mBoatTransform(nullptr)
    , mBoatOrientation(nullptr)
    , mCameraComponent(nullptr)
    , mAnchorTransform(nullptr)
    , mFlybyComponent(nullptr)
  {
    YTEUnusedArgument(aProperties);
    //will probably have some props
  }

  void CameraController::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();

    //mSpace->YTERegister(Events::LogicUpdate, this, &CameraController::OnLogicUpdate);
    mSpace->YTERegister(Events::AttachCamera, this, &CameraController::OnAttachCamera);
    mSpace->YTERegister(Events::DebugSwitch, this, &CameraController::OnDebugSwitch);
    //mSpace->YTERegister(Events::AnchorUpdate, this, &CameraController::OnAnchorUpdate);
    /*mSpace->YTERegister(Events::CameraRotateEvent, this, &CameraController::RotateCamera);
    mSpace->YTERegister(Events::DirectCameraEvent, this, &CameraController::OnDirectCamera);
    mSpace->YTERegister(Events::DialogueExit, this, &CameraController::OnDialogueExit);
    
    mTransform = mOwner->GetComponent<Transform>();
    mOrientation = mOwner->GetComponent<Orientation>();
    mCameraComponent = mOwner->GetComponent<Camera>();
    
    Composition *boat = mOwner->GetParent();
    mBoatTransform = boat->GetComponent<Transform>();
    mBoatOrientation = boat->GetComponent<Orientation>();

    mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() + glm::vec3(0.0f, 5.0f, 0.0f));
    mTransform->SetWorldRotation(glm::vec3(0.f));
    RotateOnBoom(mBoatTransform->GetWorldTranslation());*/
  }

  void CameraController::OnAttachCamera(AttachCamera *aEvent)
  {
      // Detach ourselves from the previous anchor if we must
    if (mAnchorTransform)
    {
      mAnchor->YTEDeregister(Events::PositionChanged, this, &CameraController::OnAnchorPositionUpdate);
      mAnchor->YTEDeregister(Events::RotationChanged, this, &CameraController::OnAnchorRotationUpdate);
    }

    mAnchor = aEvent->Anchor;
    mAnchorTransform = mAnchor->GetComponent<Transform>();
    mAnchor->YTERegister(Events::PositionChanged, this, &CameraController::OnAnchorPositionUpdate);
    mAnchor->YTERegister(Events::RotationChanged, this, &CameraController::OnAnchorRotationUpdate);

    mTransform->SetWorldTranslation(mAnchorTransform->GetWorldTranslation());
    mTransform->SetWorldRotation(mAnchorTransform->GetWorldRotation());
  }

  void CameraController::OnAnchorPositionUpdate(TransformChanged* aEvent)
  {
    mTransform->SetWorldTranslation(aEvent->WorldPosition);
  }

  void CameraController::OnAnchorRotationUpdate(TransformChanged* aEvent)
  {
    mTransform->SetWorldRotation(aEvent->WorldRotation);
  }

  void CameraController::OnDebugSwitch(DebugSwitch *aEvent)
  {
    if (aEvent->EnableDebug)
    {
      if (mFlybyComponent == nullptr)
      {
        if (mAnchorTransform)
        {
          mAnchor->YTEDeregister(Events::PositionChanged, this, &CameraController::OnAnchorPositionUpdate);
          mAnchor->YTEDeregister(Events::RotationChanged, this, &CameraController::OnAnchorRotationUpdate);
        }

        mFlybyComponent = mOwner->AddComponent<FlybyCamera>();
        mFlybyComponent->Initialize();
      }
    }
    else
    {
      mOwner->RemoveComponent(mFlybyComponent);
      mFlybyComponent = nullptr;

      if (mAnchorTransform != nullptr)
      {
        mAnchor->YTERegister(Events::PositionChanged, this, &CameraController::OnAnchorPositionUpdate);
        mAnchor->YTERegister(Events::RotationChanged, this, &CameraController::OnAnchorRotationUpdate);

        mTransform->SetWorldTranslation(mAnchorTransform->GetWorldTranslation());
        mTransform->SetWorldRotation(mAnchorTransform->GetWorldRotation());
      }
    }
  }

  //void CameraController::RotateCamera(CameraRotateEvent *aEvent)
  //{
  //  mRotationAngle -= aEvent->StickDirection.x / 2.0f;
  //  if (mRotationAngle > 360.0f)
  //  {
  //    mRotationAngle = 0.0f;
  //  }
  //  if (mRotationAngle < 0.0f)
  //  {
  //    mRotationAngle = 360.0f;
  //  }
  //  /*
  //  glm::vec3 camAxisAngle = glm::vec3(0, mRotationAngle, 0);

  //  glm::quat rot = glm::angleAxis(mRotationAngle, mOrientation->GetUpVector());
  //  glm::vec3 newRot = glm::rotate(rot, mOrientation->GetForwardVector());
  //  //mTransform->SetRotationProperty(newRot);
  //  mTransform->SetWorldRotationProperty(newRot);
  //  */
  //  //UBOView view;
  //  //view.mViewMatrix = glm::lookAt(mTransform->GetWorldTranslation(), mBoatTransform->GetWorldTranslation(), glm::vec3(0, 1, 0));
  //  //view.mCameraPosition = glm::vec4(mTransform->GetWorldTranslation(), 1.0f);
  //  //
  //  //mCameraComponent->SetUBOView(view);

  //  mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() + glm::vec3(0.0f, 2.0f, 0.0f));
  //  mTransform->SetWorldRotationProperty(glm::vec3(0.0f, mRotationAngle, 0.0f));
  //  glm::quat rot = mTransform->GetWorldRotation();
  //  glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 1.0f); // used to translate away from target point by 1
  //  unitVector = glm::rotate(rot, unitVector);
  //  unitVector = glm::normalize(unitVector);
  //  unitVector = 15.0f * unitVector;
  //  mTransform->SetWorldTranslation(mTransform->GetWorldTranslation() - glm::vec3(unitVector));
  //  //mOrientation->LookAtPoint(mBoatTransform->GetWorldTranslation());
  //  mTransform->RotateTowardPoint(mBoatTransform->GetWorldTranslation(), mOrientation->GetUpVector());

  //  mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() + glm::vec3(0.0f, 5.0f, 0.0f));
  //  mTransform->SetWorldRotationProperty(glm::vec3(0.0f, mRotationAngle, 0.0f));
  //  RotateOnBoom(mBoatTransform->GetWorldTranslation());

  //  //mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() - mOrientation->GetForwardVector() * 10.0f);

  //  /*
  //  glm::vec3 camForwardPoint = mTransform->GetTranslation() + camForward;
  //  glm::vec3 translationVec = mBoatTransform->GetTranslation() - camForwardPoint;
  //  //dont add the y component
  //  translationVec.y = 0.0f;
  //  
  //  mTransform->SetTranslation(mTransform->GetTranslation() + translationVec);
  //  mTransform->SetRotationProperty(camForward);
  //  */
  //}

  //void CameraController::OnDirectCamera(DirectCameraEvent *aEvent)
  //{
  //  mTransform->SetWorldTranslation(aEvent->CameraAnchor);
  //  //mTransform->SetWorldRotation(glm::vec3(0.f));

  //  auto lookAt = aEvent->LookAtPoint - aEvent->CameraAnchor;
  //  //mTransform->RotateToward(lookAt, glm::cross(lookAt, mOrientation->GetRightVector()));
  //  //RotateOnBoom(aEvent->LookAtPoint);
  //  mTransform->RotateTowardPoint(aEvent->LookAtPoint, mOrientation->GetUpVector());
  //  //mOrientation->LookAtPoint(aEvent->LookAtPoint);
  //}

  //void CameraController::OnDialogueExit(DialogueExit *aEvent)
  //{
  //  YTEUnusedArgument(aEvent);
  //  mTransform->SetWorldTranslation(mBoatTransform->GetWorldTranslation() + glm::vec3(0.0f, 5.0f, 0.0f));
  //  mTransform->SetWorldRotation(glm::vec3(0.f));
  //  //mTransform->RotateToward(mOrientation->GetForwardVector(), mBoatOrientation->GetForwardVector(), mBoatOrientation->GetUpVector());
  //  RotateOnBoom(mBoatTransform->GetWorldTranslation());
  //}

  //void CameraController::RotateOnBoom(const glm::vec3& aFocusPoint)
  //{
  //  glm::quat rot = mTransform->GetWorldRotation();
  //  glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 0.0f); // used to translate away from target point by 1
  //  unitVector = glm::rotate(rot, unitVector);
  //  unitVector = glm::normalize(unitVector);
  //  unitVector = 30.0f * unitVector;
  //  mTransform->SetWorldTranslation(mTransform->GetWorldTranslation() - glm::vec3(unitVector));
  //  mTransform->RotateTowardPoint(aFocusPoint, mOrientation->GetUpVector());
  //  //mOrientation->LookAtPoint(aFocusPoint);
  //}
}
