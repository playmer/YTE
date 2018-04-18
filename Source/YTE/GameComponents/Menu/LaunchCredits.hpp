/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/18
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  YTEDeclareEvent(StartCredits);

  class StartCredits : public Event
  {
  public:
    YTEDeclareType(StartCredits);
  };

  class LaunchCredits : public Component
  {
  public:
    YTEDeclareType(LaunchCredits);
    LaunchCredits(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnElementTrigger(MenuElementTrigger *);

  private:
  };
}