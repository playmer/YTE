/******************************************************************************/
/*!
\file   WindowsMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the Windows menu on the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qdockwidget.h>

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/WindowsMenu.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MainWindow/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/OutputConsole/OutputConsole.hpp"
#include "YTEditor/MainWindow/Widgets/WWiseViewer/WWiseWidget.hpp"


namespace YTEditor
{
  WindowsMenu::WindowsMenu(MainWindow * aMainWindow)
    : Menu("Windows", aMainWindow)
  {
    AddMenu(MakeObjectBrowserMenu());
    AddMenu(MakeComponentBrowserMenu());
    AddMenu(MakeOutputConsoleMenu());
    AddMenu(MakeMaterialViewerMenu());
    AddMenu(MakeFileViewerMenu());
    AddMenu(MakeWWiseViewerMenu());
  }

  Menu* WindowsMenu::MakeObjectBrowserMenu()
  {
    Menu *menu = new Menu("Object Browser", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenObjectBrowser, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseObjectBrowser, this);

    return menu;
  }

  void WindowsMenu::OpenObjectBrowser()
  {
    mMainWindow->GetObjectBrowserDock()->show();
  }

  void WindowsMenu::CloseObjectBrowser()
  {
    mMainWindow->GetObjectBrowserDock()->hide();
  }

  Menu* WindowsMenu::MakeComponentBrowserMenu()
  {
    Menu *menu = new Menu("Component Browser", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenComponentBrowser, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseComponentBrowser, this);

    return menu;
  }

  void WindowsMenu::OpenComponentBrowser()
  {
    mMainWindow->GetComponentBrowserDock()->show();
  }

  void WindowsMenu::CloseComponentBrowser()
  {
    mMainWindow->GetComponentBrowserDock()->hide();
  }

  Menu* WindowsMenu::MakeOutputConsoleMenu()
  {
    Menu* menu = new Menu("Output Console", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenOutputConsole, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseOutputConsole, this);

    return menu;
  }

  void WindowsMenu::OpenOutputConsole()
  {
    mMainWindow->GetOutputConsoleDock()->show();
  }

  void WindowsMenu::CloseOutputConsole()
  {
    mMainWindow->GetOutputConsoleDock()->hide();
  }

  Menu* WindowsMenu::MakeMaterialViewerMenu()
  {
    Menu *menu = new Menu("Material Viewer", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenMaterialViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseMaterialViewer, this);

    return menu;
  }

  void WindowsMenu::OpenMaterialViewer()
  {
    mMainWindow->GetMaterialViewerDock()->show();
  }

  void WindowsMenu::CloseMaterialViewer()
  {
    mMainWindow->GetMaterialViewerDock()->hide();
  }

  Menu* WindowsMenu::MakeFileViewerMenu()
  {
    Menu *menu = new Menu("File Viewer", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenFileViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseFileViewer, this);

    return menu;
  }

  void WindowsMenu::OpenFileViewer()
  {
    mMainWindow->GetFileViewerDock()->show();
  }

  void WindowsMenu::CloseFileViewer()
  {
    mMainWindow->GetFileViewerDock()->hide();
  }

  Menu* WindowsMenu::MakeWWiseViewerMenu()
  {
    Menu *menu = new Menu("WWise Viewer", mMainWindow);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenWWiseViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseWWiseViewer, this);

    return menu;
  }

  void WindowsMenu::OpenWWiseViewer()
  {
    mMainWindow->GetWWiseWidgetDock()->show();
  }

  void WindowsMenu::CloseWWiseViewer()
  {
    mMainWindow->GetWWiseWidgetDock()->hide();
  }
}
