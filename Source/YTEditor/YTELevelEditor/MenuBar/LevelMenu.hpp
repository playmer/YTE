/******************************************************************************/
/*!
\file   LevelMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for reloading the current level.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/Menu.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

namespace YTEditor
{
  class LevelMenu : public Framework::Menu
  {
  public:
    LevelMenu(Framework::MainWindow* aMainWindow);

  private:

    CompositionBrowser* mObjectBrowser;
    ComponentTree* mComponentTree;

    Menu* MakeCurrentLevelMenu();
    void ReloadCurrentLevel();

    Menu* MakeSpaceMenu();
    void SelectSpace();

    QAction* MakeSelectCameraAct();
    void SelectCamera();

    Menu* MakeEngineMenu();
    void SelectEngine();

    Menu* MakeSetLightingMenu();
    void TurnLightsOn();
    void TurnLightsOff();

  };
}
