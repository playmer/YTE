/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/GameComponents/DialogueDirector.hpp"

namespace YTE
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////
  static std::vector<std::string> ContentTypeStrings{ "Active", "Passive" };

  class DialogueElement : public Component
  {
  public:
    enum class ContentType { Active = 0, Passive, Count };

    YTEDeclareType(DialogueElement);
    DialogueElement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    // Properties /////////////////////////////////////////////////////////////////////
    const std::string& GetContentType() { return ContentTypeStrings[static_cast<int>(mContentType)]; }
    void SetContentType(const std::string& aType)
    {
      for (auto i = 0; i < static_cast<int>(ContentType::Count); ++i)
      {
        if (aType.compare(ContentTypeStrings[i]) == 0)
        {
          mContentType = static_cast<ContentType>(static_cast<int>(ContentType::Active) + i);
        }
      }
    }

    const int GetSelectionIndex() { return mSelectionIndex; }
    void SetSelectionIndex(int aIndex) { mSelectionIndex = aIndex; }
    ///////////////////////////////////////////////////////////////////////////////////

    void OnStart(LogicUpdate*);
    void OnContentUpdate(UIUpdateContent *aEvent);
    void OnFocusSwitch(UIFocusSwitchEvent *aEvent);
    void OnDisplayEvent(UIDisplayEvent *aEvent);
    void OnSelectEvent(UISelectEvent *aEvent);

  private:
    Sprite *mMySprite;
    Sprite *mAcceptSprite;
    Sprite *mNextSprite;
    Sprite *mPrevSprite;
    SpriteText *mTextContent;

    ContentType mContentType;
    int mSelectionIndex;
    bool mIsDisplayed;

    void UpdateVisibility(bool aBecomeVisible);
  };
}
