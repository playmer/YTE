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
    , mTurnScale(0.0f)
    , mTurnDirection(glm::vec2(0.0f))
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CameraBoom::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();

    mSpace->YTERegister(Events::LogicUpdate, this, &CameraBoom::OnLogicUpdate);
    mSpace->YTERegister(Events::CameraRotateEvent, this, &CameraBoom::OnCameraRotate);
  }

  void CameraBoom::OnLogicUpdate(LogicUpdate *aEvent)
  {
    float floatDt = static_cast<float>(aEvent->Dt);

    mTransform->RotateAboutWorldAxis(glm::vec3(0.0f, 1.0f, 0.0f), mTurnScale * mCamTurnSpeed.x * floatDt * mTurnDirection.x);
    mTransform->RotateAboutLocalAxis(glm::vec3(1.0f, 0.0f, 0.0f), mTurnScale * mCamTurnSpeed.y * floatDt * mTurnDirection.y);
  }

  void CameraBoom::OnCameraRotate(CameraRotateEvent *aEvent)
  {
    mTurnDirection = aEvent->StickDirection;

      // Dead-zone check and apply response curves
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.01f)
    {
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
    }
  }
}
