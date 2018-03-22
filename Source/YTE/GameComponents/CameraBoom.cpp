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
  }

  CameraBoom::CameraBoom(Composition *aOwner, Space *aSpace, RSValue *)
    : Component(aOwner, aSpace)
    , mTransform(nullptr)
    , mOrientation(nullptr)
    , mRotScale(0.0f)
    , mRotDirection(glm::vec2(0.0f))
  {
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

    mTransform->RotateAboutWorldAxis(glm::vec3(0.0f, 1.0f, 0.0f), mRotScale * floatDt * mRotDirection.x);
    mTransform->RotateAboutLocalAxis(glm::vec3(1.0f, 0.0f, 0.0f), mRotScale * floatDt * mRotDirection.y);
  }

  void CameraBoom::OnCameraRotate(CameraRotateEvent *aEvent)
  {
    mRotDirection = aEvent->StickDirection;

      // Dead-zone check and apply response curves
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.01f)
    {
      //float rotateScale;
      float startVal = 0.0f;
      float change = 1.0f / 0.5f;
      float duration = 1.0f;

      if (length < 0.5f)
      {
        Quad::easeIn::Ease(mRotScale, startVal, change, length, duration);
      }
      else if (length < 0.7f)
      {
        mRotScale = length;
      }
      else
      {
        // Have to manipulate the parabola to line up with our piecewise function correctly
        float vertOffset = 1.0f;
        change = 1.0f / 0.3f;
        Quad::piecewiseEaseOut::Ease(mRotScale, vertOffset, change, length, duration);
      }

      //mTransform->RotateAboutWorldAxis(glm::vec3(0.0f, 1.0f, 0.0f), mRotScale);
      //mOrientation->LookAt(glm::vec3(aEvent->StickDirection.x, aEvent->StickDirection.y, mOrientation->GetForwardVector().z));
    }
    else
    {
      mRotScale = 0.0f;
    }
  }
}
