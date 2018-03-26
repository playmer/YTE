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

  CameraController::CameraController(Composition *aOwner, Space *aSpace, RSValue *)
    : Component(aOwner, aSpace)
    , mTransform(nullptr)
    , mAnchorTransform(nullptr)
    , mFlybyComponent(nullptr)
  {
  }

  void CameraController::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();

    mSpace->YTERegister(Events::AttachCamera, this, &CameraController::OnAttachCamera);
    mSpace->YTERegister(Events::DebugSwitch, this, &CameraController::OnDebugSwitch);
  }

  void CameraController::OnAttachCamera(AttachCamera *aEvent)
  {
      // Detach ourselves from the previous anchor if we must
    if (mAnchor)
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
}
