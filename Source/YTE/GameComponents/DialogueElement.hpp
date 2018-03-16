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
  class DialogueElement : public Component
  {
  public:
    YTEDeclareType(DialogueElement);
    DialogueElement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    // Properties /////////////////////////////////////////////////////////////////////
    bool GetCanDeselect() { return mCanDeselect; }
    void SetCanDeselect(bool aCanDeselect) { mCanDeselect = aCanDeselect; }
    ///////////////////////////////////////////////////////////////////////////////////

    void OnDisplayEvent(UIDisplayEvent *aEvent);
    void OnContentUpdate(UIUpdateContent *aEvent);
    void OnSelectEvent(UISelectEvent *aEvent);
    void OnConfirmEvent(UIConfirmEvent *aEvent);

  private:
    Transform *mTransform;
    SpriteText *mText;

    glm::vec3 mInitialPos;

    std::string mDeselectString = "...";
    std::string mSelectString;

    bool mCanDeselect;
  };
}
