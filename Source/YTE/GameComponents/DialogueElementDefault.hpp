/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/25
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/GameComponents/DialogueDirector.hpp"

namespace YTE
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////
  class DialogueElementDefault : public Component
  {
  public:
    YTEDeclareType(DialogueElementDefault);
    DialogueElementDefault(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // Properties /////////////////////////////////////////////////////////////////////
    const int GetSelectionIndex() { return mSelectionIndex; }
    void SetSelectionIndex(int aIndex) { mSelectionIndex = aIndex; }
    ///////////////////////////////////////////////////////////////////////////////////

    void OnStart(LogicUpdate*);
    void OnDisplayEvent(UIDisplayEvent *aEvent);
    void OnSelectEvent(UISelectEvent *aEvent);

  private:
    Sprite *mMySprite;
    SpriteText *mTextContent;

    int mSelectionIndex;
    bool mIsDisplayed;

    void UpdateVisibility(bool aBecomeVisible);
  };
}
