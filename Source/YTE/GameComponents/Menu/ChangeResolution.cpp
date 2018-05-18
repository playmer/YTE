/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/18
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/GameComponents/Menu/ChangeResolution.hpp"

namespace YTE
{
  YTEDefineType(ChangeResolution)
  {
    RegisterType<ChangeResolution>();
    TypeBuilder<ChangeResolution> builder;
  }

  ChangeResolution::ChangeResolution(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mMyTransform(nullptr)
    , mArrows()
    , mFieldText(nullptr)
    , mCurrWidth(0)
    , mCurrHeight(0)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ChangeResolution::Initialize()
  {
    mMyTransform = mOwner->GetComponent<Transform>();

    if (Composition *left = mOwner->FindFirstCompositionByName("LeftArrow"))
    {
      mArrows.mLeft = left->GetComponent<Sprite>();
    }
    if (Composition *right = mOwner->FindFirstCompositionByName("RightArrow"))
    {
      mArrows.mRight = right->GetComponent<Sprite>();
    }
    if (Composition *text = mOwner->FindFirstCompositionByName("Text"))
    {
      mFieldText = text->GetComponent<SpriteText>();
    }

    mOwner->RegisterEvent<&ChangeResolution::OnElementTrigger>(Events::MenuElementTrigger, this);
  }

  void ChangeResolution::Start()
  {
    auto window = mSpace->GetComponent<GraphicsView>()->GetWindow();

    mCurrWidth = window->GetWidth();
    mCurrHeight = window->GetHeight();

    if (mFieldText)
    {
      std::string s = fmt::format("{0}x{1}", mCurrWidth, mCurrHeight);
      mFieldText->SetText(s);
    }

    if (mCurrWidth == 1152 && mCurrHeight == 648)
    {
      mCurrResolution = Resolutions::R1152x648;
    }
    else if (mCurrWidth == 1280 && mCurrHeight == 720)
    {
      mCurrResolution = Resolutions::R1280x720;
    }
    else if (mCurrWidth == 2160 && mCurrHeight == 1440)
    {
      mCurrResolution = Resolutions::R2160x1440;
    }
    else
    {
      mCurrResolution = Resolutions::R1920x1080;
    }
  }

  void ChangeResolution::OnStickFlickEvent(OptionsFlickEvent *aEvent)
  {
    if (aEvent->StickDirection.x > 0.0f)
    {
      mCurrResolution = Resolutions::Type((mCurrResolution + 1) % Resolutions::COUNT);
    }
    else if (aEvent->StickDirection.x < 0.0f)
    {
      mCurrResolution = Resolutions::Type((mCurrResolution + Resolutions::COUNT - 1) % Resolutions::COUNT);
    }

    if (mFieldText)
    {
      mFieldText->SetText(Resolutions::Names[mCurrResolution]);
    }
  }

  void ChangeResolution::OnConfirm(OptionsConfirmEvent *)
  {
    mMyTransform->SetScale(mMyTransform->GetScale() / 1.2f);

    if (mArrows.mLeft)
    {
      mArrows.mLeft->SetVisibility(false);
    }
    if (mArrows.mRight)
    {
      mArrows.mRight->SetVisibility(false);
    }

    switch (mCurrResolution)
    {
      case Resolutions::R1152x648:
      {
        mCurrWidth = 1152;
        mCurrHeight = 648;
        break;
      }

      case Resolutions::R1280x720:
      {
        mCurrWidth = 1280;
        mCurrHeight = 720;
        break;
      }

      case Resolutions::R1920x1080:
      {
        mCurrWidth = 1920;
        mCurrHeight = 1080;
        break;
      }

      case Resolutions::R2160x1440:
      {
        mCurrWidth = 2160;
        mCurrHeight = 1440;
        break;
      }
    }

    auto window = mSpace->GetComponent<GraphicsView>()->GetWindow();
    window->SetResolution(mCurrWidth, mCurrHeight);

    mSpace->DeregisterEvent<&ChangeResolution::OnConfirm>(Events::OptionsConfirmEvent,  this);
    mSpace->DeregisterEvent<&ChangeResolution::OnStickFlickEvent>(Events::OptionsFlickEvent,  this);
    mSpace->GetOwner()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Menu);
  }

  void ChangeResolution::OnElementTrigger(MenuElementTrigger *)
  {
    mMyTransform->SetScale(1.2f * mMyTransform->GetScale());

    if (mArrows.mLeft)
    {
      mArrows.mLeft->SetVisibility(true);
    }
    if (mArrows.mRight)
    {
      mArrows.mRight->SetVisibility(true);
    }

    mSpace->RegisterEvent<&ChangeResolution::OnConfirm>(Events::OptionsConfirmEvent, this);
    mSpace->RegisterEvent<&ChangeResolution::OnStickFlickEvent>(Events::OptionsFlickEvent, this);
    mSpace->GetOwner()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Options);
  }
}
