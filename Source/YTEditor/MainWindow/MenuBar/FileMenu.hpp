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

#include "YTEditor/MainWindow/MenuBar/Menu.hpp"

namespace YTEditor
{
  class FileMenu : public Menu
  {
  public:
    FileMenu(MainWindow *aMainWindow);

  private:
    void NewLevel();
    void OpenLevel();
    void SaveLevel();
    void SaveLevelAs();

    void OpenFile();

    void ExitEditor();
  };

}