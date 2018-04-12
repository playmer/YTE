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

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  YTEDeclareEvent(StartGame);

  class StartGame : public Event
  {
  public:
    YTEDeclareType(StartGame);
  };

  class LaunchGame : public Component
  {
  public:
    YTEDeclareType(LaunchGame);
    LaunchGame(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnElementTrigger(MenuElementTrigger *);

  private:
  };
}