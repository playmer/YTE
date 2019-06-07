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

#include "YTEditor/YTELevelEditor/MenuBar/WindowsMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"
#include "YTEditor/YTELevelEditor/Widgets/WWiseViewer/WWiseWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/FileViewer/FileViewer.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{
  WindowsMenu::WindowsMenu(Framework::MainWindow* aMainWindow)
    : Menu("Windows", aMainWindow->GetWorkspace<YTELevelEditor>())
  {
    AddMenu(MakeObjectBrowserMenu());
    AddMenu(MakeComponentBrowserMenu());
    AddMenu(MakeOutputConsoleMenu());
    AddMenu(MakeMaterialViewerMenu());
    AddMenu(MakeFileViewerMenu());
    AddMenu(MakeWWiseViewerMenu());
  }

  Framework::Menu* WindowsMenu::MakeObjectBrowserMenu()
  {
    Menu *menu = new Framework::Menu("Object Browser", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenObjectBrowser, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseObjectBrowser, this);

    return menu;
  }

  void WindowsMenu::OpenObjectBrowser()
  {
    mWorkspace->GetWidget<ObjectBrowser>()->show();
  }

  void WindowsMenu::CloseObjectBrowser()
  {
    mWorkspace->GetWidget<ObjectBrowser>()->hide();
  }

  Framework::Menu* WindowsMenu::MakeComponentBrowserMenu()
  {
    auto menu = new Framework::Menu("Component Browser", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenComponentBrowser, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseComponentBrowser, this);

    return menu;
  }

  void WindowsMenu::OpenComponentBrowser()
  {
    mWorkspace->GetWidget<ComponentBrowser>()->show();
  }

  void WindowsMenu::CloseComponentBrowser()
  {
    mWorkspace->GetWidget<ComponentBrowser>()->hide();
  }

  Framework::Menu* WindowsMenu::MakeOutputConsoleMenu()
  {
    auto menu = new Framework::Menu("Output Console", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenOutputConsole, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseOutputConsole, this);

    return menu;
  }

  void WindowsMenu::OpenOutputConsole()
  {
    mWorkspace->GetWidget<OutputConsole>()->show();
  }

  void WindowsMenu::CloseOutputConsole()
  {
    mWorkspace->GetWidget<OutputConsole>()->hide();
  }

  Framework::Menu* WindowsMenu::MakeMaterialViewerMenu()
  {
    auto menu = new Framework::Menu("Material Viewer", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenMaterialViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseMaterialViewer, this);

    return menu;
  }

  void WindowsMenu::OpenMaterialViewer()
  {
    mWorkspace->GetWidget<MaterialViewer>()->show();
  }

  void WindowsMenu::CloseMaterialViewer()
  {
    mWorkspace->GetWidget<MaterialViewer>()->hide();
  }

  Framework::Menu* WindowsMenu::MakeFileViewerMenu()
  {
    auto menu = new Framework::Menu("File Viewer", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenFileViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseFileViewer, this);

    return menu;
  }

  void WindowsMenu::OpenFileViewer()
  {
    mWorkspace->GetWidget<FileViewer>()->show();
  }

  void WindowsMenu::CloseFileViewer()
  {
    mWorkspace->GetWidget<FileViewer>()->hide();
  }

  Framework::Menu* WindowsMenu::MakeWWiseViewerMenu()
  {
    auto menu = new Framework::Menu("WWise Viewer", mWorkspace);

    menu->AddAction<WindowsMenu>("Show", &WindowsMenu::OpenWWiseViewer, this);
    menu->AddAction<WindowsMenu>("Hide", &WindowsMenu::CloseWWiseViewer, this);

    return menu;
  }

  void WindowsMenu::OpenWWiseViewer()
  {
    mWorkspace->GetWidget<WWiseWidget>()->show();
  }

  void WindowsMenu::CloseWWiseViewer()
  {
    mWorkspace->GetWidget<WWiseWidget>()->hide();
  }
}
