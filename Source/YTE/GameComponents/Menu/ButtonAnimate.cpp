/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/20
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/Menu/ButtonAnimate.hpp"

namespace YTE
{
  YTEDefineType(ButtonAnimate)
  {
    RegisterType<ButtonAnimate>();

    YTEBindProperty(&GetScaleFactor, &SetScaleFactor, "ScaleFactor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Factor by which the UI element scales when hovered");
  }

  ButtonAnimate::ButtonAnimate(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mScaleFactor(1.0f), mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());

    mConstructing = false;
  }

  void ButtonAnimate::Initialize()
  {
    mMyTransform = mOwner->GetComponent<Transform>();

    //mSpace->YTERegister(Events::LogicUpdate, this, &ButtonAnimate::OnStart);

    mOwner->YTERegister(Events::MenuElementHover, this, &ButtonAnimate::OnButtonHover);
    mOwner->YTERegister(Events::MenuElementDeHover, this, &ButtonAnimate::OnButtonDeHover);
  }

  void ButtonAnimate::Start()
  {
    mNeutralScale = mMyTransform->GetScale();
    //mSpace->YTEDeregister(Events::LogicUpdate, this, &ButtonAnimate::OnStart);
  }

  void ButtonAnimate::OnButtonHover(MenuElementHover* aEvent)
  {
    YTEUnusedArgument(aEvent);
    mMyTransform->SetScale(mScaleFactor * mNeutralScale);
  }

  void ButtonAnimate::OnButtonDeHover(MenuElementDeHover* aEvent)
  {
    YTEUnusedArgument(aEvent);
    mMyTransform->SetScale(mNeutralScale);
  }
}
