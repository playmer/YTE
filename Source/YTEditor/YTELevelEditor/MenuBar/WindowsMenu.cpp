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

#include "YTEditor/YTELevelEditor/MainWindow.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/WindowsMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"
#include "YTEditor/YTELevelEditor/Widgets/WWiseViewer/WWiseWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/FileViewer/FileViewer.hpp"


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
    mMainWindow->GetWidget<ObjectBrowser>()->show();
  }

  void WindowsMenu::CloseObjectBrowser()
  {
    mMainWindow->GetWidget<ObjectBrowser>()->hide();
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
    mMainWindow->GetWidget<ComponentBrowser>()->show();
  }

  void WindowsMenu::CloseComponentBrowser()
  {
    mMainWindow->GetWidget<ComponentBrowser>()->hide();
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
    mMainWindow->GetWidget<OutputConsole>()->show();
  }

  void WindowsMenu::CloseOutputConsole()
  {
    mMainWindow->GetWidget<OutputConsole>()->hide();
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
    mMainWindow->GetWidget<MaterialViewer>()->show();
  }

  void WindowsMenu::CloseMaterialViewer()
  {
    mMainWindow->GetWidget<MaterialViewer>()->hide();
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
    mMainWindow->GetWidget<FileViewer>()->show();
  }

  void WindowsMenu::CloseFileViewer()
  {
    mMainWindow->GetWidget<FileViewer>()->hide();
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
    mMainWindow->GetWidget<WWiseWidget>()->show();
  }

  void WindowsMenu::CloseWWiseViewer()
  {
    mMainWindow->GetWidget<WWiseWidget>()->hide();
  }
}
