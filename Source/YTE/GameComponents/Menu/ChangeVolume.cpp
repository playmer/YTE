/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/17
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Actions/Action.hpp"

#include "YTE/WWise/WWiseEmitter.hpp"

#include "YTE/GameComponents/Menu/ChangeVolume.hpp"

namespace YTE
{
  YTEDefineType(ChangeVolume)
  {
    RegisterType<ChangeVolume>();
  }

  ChangeVolume::ChangeVolume(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mSoundSystem(nullptr)
    , mMyTransform(nullptr)
    , mMaskTransform(nullptr)
    , mSliderRange()
    , mLastStickX(0.0f)
    , mFillRatio(0.99f)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ChangeVolume::Initialize()
  {
    mSoundSystem = mSpace->GetEngine()->GetComponent<WWiseSystem>();

    mMyTransform = mOwner->GetComponent<Transform>();
    
    if (Composition *fill = mOwner->GetParent()->FindFirstCompositionByName("SliderMask"))
    {
      mMaskTransform = fill->GetComponent<Transform>();
    }

    mSpace->YTERegister(Events::LogicUpdate, this, &ChangeVolume::OnLogicUpdate);
    mOwner->YTERegister(Events::MenuElementTrigger, this, &ChangeVolume::OnElementTrigger);
  }

  void ChangeVolume::Start()
  {
    auto sliderTransform = mOwner->GetParent()->FindFirstCompositionByName("Slider")->GetComponent<Transform>();

    mSliderRange = { (sliderTransform->GetWorldTranslation().x - (sliderTransform->GetWorldScale().x / 2.0f)),
                     (sliderTransform->GetWorldTranslation().x + (sliderTransform->GetWorldScale().x / 2.0f)) };;

    UpdateSlider();
  }

  void ChangeVolume::OnLogicUpdate(LogicUpdate *aEvent)
  {
    if (mLastStickX != 0.0f)
    {
      mFillRatio += (mLastStickX * static_cast<float>(aEvent->Dt));

      if (mFillRatio > 0.99f)
      {
        mFillRatio = 0.99f;
      }
      else if (mFillRatio < 0.01f)
      {
        mFillRatio = 0.01f;
      }
      else
      {
        UpdateSlider();
      }

      if (mSoundSystem)
      {
        mSoundSystem->SetRTPC("MasterVolume", mFillRatio * 100.f);
      }
    }
  }

  void ChangeVolume::OnStickEvent(OptionsStickEvent *aEvent)
  {
    float length = glm::length(aEvent->StickDirection);

    if (length > 0.01f)
    {
      mLastStickX = aEvent->StickDirection.x;
    }
    else
    {
      mLastStickX = 0.0f;
    }
  }

  void ChangeVolume::OnConfirm(OptionsConfirmEvent *)
  {
    mLastStickX = 0.0f;
    mMyTransform->SetScale(mMyTransform->GetScale() / 1.2f);

    mSpace->YTEDeregister(Events::OptionsConfirmEvent, this, &ChangeVolume::OnConfirm);
    mSpace->YTEDeregister(Events::OptionsStickEvent, this, &ChangeVolume::OnStickEvent);
    mSpace->GetOwner()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Menu);
  }

  void ChangeVolume::OnElementTrigger(MenuElementTrigger *)
  {
    mMyTransform->SetScale(1.2f * mMyTransform->GetScale());

    mSpace->YTERegister(Events::OptionsConfirmEvent, this, &ChangeVolume::OnConfirm);
    mSpace->YTERegister(Events::OptionsStickEvent, this, &ChangeVolume::OnStickEvent);
    mSpace->GetOwner()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Options);
  }

  void ChangeVolume::UpdateSlider()
  {
    glm::vec3 newPosition((1.0f - mFillRatio) * mSliderRange.x + mFillRatio * mSliderRange.y, mMyTransform->GetWorldTranslation().y, mMyTransform->GetWorldTranslation().z);

    mMyTransform->SetWorldTranslation(newPosition);

    mMaskTransform->SetWorldScale((1.0f - mFillRatio) * (mSliderRange.y - mSliderRange.x), mMaskTransform->GetWorldScale().y, mMaskTransform->GetWorldScale().z);
    mMaskTransform->SetWorldTranslation(newPosition.x + 0.5f * (mSliderRange.y - newPosition.x), mMaskTransform->GetWorldTranslation().y, mMaskTransform->GetWorldTranslation().z);
  }
}
