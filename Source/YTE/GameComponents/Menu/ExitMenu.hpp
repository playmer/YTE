/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/11
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  class ExitMenu : public Component
  {
  public:
    YTEDeclareType(ExitMenu);
    ExitMenu(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    bool GetShouldCloseAll() { return mShouldCloseAll; }
    void SetShouldCloseAll(bool aShouldCloseAll) { mShouldCloseAll = aShouldCloseAll; }
    ////////////////////////////////////////////////////////

    void OnElementTrigger(MenuElementTrigger *);

  private:
    bool mShouldCloseAll;
  };
}