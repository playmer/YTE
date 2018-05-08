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
    RegisterType<CameraController>();
    TypeBuilder<CameraController> builder;

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

    mSpace->RegisterEvent<&CameraController::OnAttachCamera>(Events::AttachCamera, this);
    mSpace->RegisterEvent<&CameraController::OnDebugSwitch>(Events::DebugSwitch, this);
  }

  void CameraController::OnAttachCamera(AttachCamera *aEvent)
  {
      // Detach ourselves from the previous anchor if we must
    if (mAnchor)
    {
      mAnchor->DeregisterEvent<&CameraController::OnAnchorPositionUpdate>(Events::PositionChanged,  this);
      mAnchor->DeregisterEvent<&CameraController::OnAnchorRotationUpdate>(Events::RotationChanged,  this);
    }

    mAnchor = aEvent->Anchor;
    mAnchorTransform = mAnchor->GetComponent<Transform>();
    mAnchor->RegisterEvent<&CameraController::OnAnchorPositionUpdate>(Events::PositionChanged, this);
    mAnchor->RegisterEvent<&CameraController::OnAnchorRotationUpdate>(Events::RotationChanged, this);

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
          mAnchor->DeregisterEvent<&CameraController::OnAnchorPositionUpdate>(Events::PositionChanged,  this);
          mAnchor->DeregisterEvent<&CameraController::OnAnchorRotationUpdate>(Events::RotationChanged,  this);
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
        mAnchor->RegisterEvent<&CameraController::OnAnchorPositionUpdate>(Events::PositionChanged, this);
        mAnchor->RegisterEvent<&CameraController::OnAnchorRotationUpdate>(Events::RotationChanged, this);

        mTransform->SetWorldTranslation(mAnchorTransform->GetWorldTranslation());
        mTransform->SetWorldRotation(mAnchorTransform->GetWorldRotation());
      }
    }
  }
}
