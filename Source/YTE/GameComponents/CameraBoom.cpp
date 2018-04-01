/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/22
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Actions/Action.hpp"

#include "YTE/GameComponents/CameraBoom.hpp"

namespace YTE
{
  YTEDefineType(CameraBoom)
  {
    YTERegisterType(CameraBoom);

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() },
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&GetCameraTurnSpeed, &SetCameraTurnSpeed, "CameraTurnSpeed")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  CameraBoom::CameraBoom(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mTransform(nullptr)
    , mOrientation(nullptr)
    , mCamTurnSpeed(glm::vec2(-4.0f, -2.0f))
    , mIsTurning(false)
    , mTurnScale(0.0f)
    , mTurnDirection(glm::vec2(0.0f))
    , mLastGoodRotation(glm::quat())
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CameraBoom::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();
    mOrientation = mOwner->GetComponent<Orientation>();

    mSpace->YTERegister(Events::LogicUpdate, this, &CameraBoom::OnStart);
    mSpace->YTERegister(Events::LogicUpdate, this, &CameraBoom::OnLogicUpdate);
    mOwner->GetOwner()->YTERegister(Events::RotationChanged, this, &CameraBoom::OnParentRotated);
    mSpace->YTERegister(Events::CameraRotateEvent, this, &CameraBoom::OnCameraRotate);
  }

  void CameraBoom::OnStart(LogicUpdate*)
  {
    mLastGoodRotation = mTransform->GetRotation();
    mSpace->YTEDeregister(Events::LogicUpdate, this, &CameraBoom::OnStart);
  }

  void CameraBoom::OnLogicUpdate(LogicUpdate *aEvent)
  {
    if (mIsTurning)
    {
      float floatDt = static_cast<float>(aEvent->Dt);

      mTransform->RotateAboutWorldAxis(glm::vec3(0.0f, 1.0f, 0.0f), mTurnScale * mCamTurnSpeed.x * floatDt * mTurnDirection.x);

      float angleBetween = glm::degrees(glm::acos(glm::dot(mOrientation->GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
      
      if ((mTurnDirection.y > 0.0f && angleBetween > 70.0f) || 
          (mTurnDirection.y < 0.0f && angleBetween < 145.0f))
      {
        mTransform->RotateAboutLocalAxis(glm::vec3(1.0f, 0.0f, 0.0f), mTurnScale * mCamTurnSpeed.y * floatDt * mTurnDirection.y);
      }

      mLastGoodRotation = mTransform->GetRotation();
    }
  }

  void CameraBoom::OnParentRotated(TransformChanged *aEvent)
  {
    if (!mIsTurning)
    {
      mLastGoodRotation = mTransform->GetRotation();
    }
    else
    {
      mTransform->SetRotation(mLastGoodRotation);
    }
  }

  void CameraBoom::OnCameraRotate(CameraRotateEvent *aEvent)
  {
    mTurnDirection = aEvent->StickDirection;

      // Dead-zone check and apply response curves
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.01f)
    {
      mIsTurning = true;

      float startVal = 0.0f;
      float change = 1.0f / 0.5f;
      float duration = 1.0f;

      if (length < 0.5f)
      {
        Quad::easeIn::Ease(mTurnScale, startVal, change, length, duration);
      }
      else if (length < 0.7f)
      {
        mTurnScale = length;
      }
      else
      {
        // Have to manipulate the parabola to line up with our piecewise function correctly
        float vertOffset = 1.0f;
        change = 1.0f / 0.3f;
        Quad::piecewiseEaseOut::Ease(mTurnScale, vertOffset, change, length, duration);
      }
    }
    else
    {
      mTurnScale = 0.0f;
      mIsTurning = false;
    }
  }
}
