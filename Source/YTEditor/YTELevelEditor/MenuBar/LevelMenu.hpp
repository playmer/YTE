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

#include "YTEditor/YTELevelEditor/MenuBar/Menu.hpp"

namespace YTEditor
{
  class ObjectBrowser;
  class ComponentTree;

  class LevelMenu : public Menu
  {
  public:
    LevelMenu(MainWindow *aMainWindow);

  private:

    ObjectBrowser *mObjectBrowser;
    ComponentTree *mComponentTree;

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
