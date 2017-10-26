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

#include <qmenu.h>

namespace YTEditor
{

  class MainWindow;

  class LevelMenu : public QMenu
  {
  public:

    LevelMenu(MainWindow *aMainWindow);
    ~LevelMenu();

  private:

    QMenu* MakeCurrentLevelMenu();
    void ReloadCurrentLevel();

    QMenu* MakeSpaceMenu();
    void SelectSpace();

    QMenu* MakeEngineMenu();
    void SelectEngine();

    MainWindow *mMainWindow;

  };
}
