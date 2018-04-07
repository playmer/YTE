/******************************************************************************/
/*!
\file   EditMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The Edit menu bar drop down.  Currently empty.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/


#pragma once

#include <qmenu.h>

namespace YTEditor
{

  class MainWindow;

  class EditMenu : public QMenu
  {
  public:

    EditMenu(MainWindow *aMainWindow);
    ~EditMenu();

  private:

    MainWindow *mMainWindow;

    void Undo();
    void Redo();

  };

}