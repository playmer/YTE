/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/17
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Sprite.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  class ToggleMute : public Component
  {
  public:
    YTEDeclareType(ToggleMute);
    ToggleMute(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnElementTrigger(MenuElementTrigger *);
    void OnParentVisibilityUpdated(UpdateVisibilityEvent *aEvent);

  private:
    Sprite *mCheckSprite;

    bool mIsMuted;
  };
}