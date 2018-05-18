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
    TypeBuilder<ButtonAnimate> builder;

    builder.Property<&GetScaleFactor, &SetScaleFactor>( "ScaleFactor")
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

    //mSpace->RegisterEvent<&ButtonAnimate::OnStart>(Events::LogicUpdate, this);

    mOwner->RegisterEvent<&ButtonAnimate::OnButtonHover>(Events::MenuElementHover, this);
    mOwner->RegisterEvent<&ButtonAnimate::OnButtonDeHover>(Events::MenuElementDeHover, this);
  }

  void ButtonAnimate::Start()
  {
    mNeutralScale = mMyTransform->GetScale();
    //mSpace->DeregisterEvent<&ButtonAnimate::OnStart>(Events::LogicUpdate,  this);
  }

  void ButtonAnimate::OnButtonHover(MenuElementHover* aEvent)
  {
    UnusedArguments(aEvent);
    mMyTransform->SetScale(mScaleFactor * mNeutralScale);
  }

  void ButtonAnimate::OnButtonDeHover(MenuElementDeHover* aEvent)
  {
    UnusedArguments(aEvent);
    mMyTransform->SetScale(mNeutralScale);
  }
}
