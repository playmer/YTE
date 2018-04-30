/******************************************************************************/
/*!
\file   EditMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the edit menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/Menu.hpp"

namespace YTEditor
{
  Menu::Menu(const char* aHeader, MainWindow *aMainWindow)
  : QMenu(aHeader), mMainWindow(aMainWindow)
  {
    setToolTipsVisible(true);
  }

  void Menu::AddMenu(Menu * aMenu)
  {
    addMenu(aMenu);
  }
}
