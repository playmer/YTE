/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/02/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/Menu/Button.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Textures/Originals";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }

  YTEDefineType(Button)
  {
    RegisterType<Button>();

    YTEBindProperty(&Button::GetHover, &Button::SetHover, "HoverSprite")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Button::GetNeutral, &Button::SetNeutral, "NeutralSprite")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Button::GetActivated, &Button::SetActivated, "ActivatedSprite")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);
  }

  Button::Button(Composition* aOwner, Space* aSpace, RSValue* aProperties) : Component(aOwner, aSpace), mConstructing(true)
  {
    DeserializeByType(aProperties, this, GetStaticType());

    mConstructing = false;
  }

  void Button::Initialize()
  {
    mOwner->RegisterEvent<&Button::OnButtonHover>(Events::MenuElementHover, this);
    mOwner->RegisterEvent<&Button::OnButtonTrigger>(Events::MenuElementTrigger, this);
    mOwner->RegisterEvent<&Button::OnButtonDeHover>(Events::MenuElementDeHover, this);

    mCurrentSprite = mOwner->GetComponent<Sprite>();
  }

  void Button::OnButtonHover(MenuElementHover* aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (mCurrentSprite != nullptr && !mHoverSpriteName.empty())
    {
      mCurrentSprite->SetTexture(mHoverSpriteName);
    }
  }

  void Button::OnButtonTrigger(MenuElementTrigger* aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (mCurrentSprite != nullptr && !mActivatedSpriteName.empty())
    {
      mCurrentSprite->SetTexture(mActivatedSpriteName);
    }
  }

  void Button::OnButtonDeHover(MenuElementDeHover* aEvent)
  {
    YTEUnusedArgument(aEvent);
    if (mCurrentSprite != nullptr && !mNeutralSpriteName.empty())
    {
      mCurrentSprite->SetTexture(mNeutralSpriteName);
    }
  }
}
