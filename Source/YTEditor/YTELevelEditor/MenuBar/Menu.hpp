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

  class Menu : public QMenu
  {
  public:

    Menu(const char* aHeader, MainWindow *aMainWindow);

    template<typename T>
    void AddAction(const char *aHeader, void (T::*aFn)(), Menu *aMenu, const char *aTooltip = "");
    
    void AddMenu(Menu *aMenu);
  
  protected:
    MainWindow *mMainWindow;
  };

  template<typename T>
  void Menu::AddAction(const char *aHeader, void(T::*aFn)(), Menu *aMenu, const char *aTooltip)
  {
    QAction *action = new QAction(aHeader);
    addAction(action);
    connect(action, &QAction::triggered, static_cast<T*>(aMenu), aFn);
    action->setToolTip(aTooltip);
  }
}