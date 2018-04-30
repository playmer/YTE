/******************************************************************************/
/*!
\file   FileMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for creating, opening, and saving files.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qmenu.h>

namespace YTEditor
{
  class MainWindow;

  class FileMenu : public QMenu
  {
  public:

    FileMenu(MainWindow *aMainWindow);
    ~FileMenu();

    void NewLevel();
    void SaveLevel();
    void SaveLevelAs();
    void ExitEditor();

  private:

    MainWindow *mMainWindow;

    void OpenLevel();
    void OpenFile();
  };

}