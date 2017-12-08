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

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MenuBar/WindowsMenu.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"
#include "YTEditor/WWiseViewer/WWiseWidget.hpp"


namespace YTEditor
{

  WindowsMenu::WindowsMenu(MainWindow * aMainWindow)
    : QMenu("Windows"), mMainWindow(aMainWindow)
  {
    addMenu(MakeObjectBrowserMenu());
    addMenu(MakeComponentBrowserMenu());
    addMenu(MakeOutputConsoleMenu());
    addMenu(MakeMaterialViewerMenu());
    addMenu(MakeFileViewerMenu());
    addMenu(MakeWWiseViewerMenu());
  }

  WindowsMenu::~WindowsMenu()
  {
  }


  QMenu* WindowsMenu::MakeObjectBrowserMenu()
  {
    QMenu *menu = new QMenu("Object Browser");

    QAction *openAct = new QAction("Open");
    menu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenObjectBrowser);

    QAction *closeAct = new QAction("Exit");
    menu->addAction(closeAct);
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseObjectBrowser);

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

  QMenu* WindowsMenu::MakeComponentBrowserMenu()
  {
    QMenu *menu = new QMenu("Component Browser");

    QAction *openAct = new QAction("Open");
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenComponentBrowser);
    menu->addAction(openAct);

    QAction *closeAct = new QAction("Exit");
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseComponentBrowser);
    menu->addAction(closeAct);

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

  QMenu* WindowsMenu::MakeOutputConsoleMenu()
  {
    QMenu* menu = new QMenu("Output Console");

    QAction *openAct = new QAction("Open");
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenOutputConsole);
    menu->addAction(openAct);

    QAction *closeAct = new QAction("Exit");
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseOutputConsole);
    menu->addAction(closeAct);

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

  QMenu* WindowsMenu::MakeMaterialViewerMenu()
  {
    QMenu *menu = new QMenu("Material Viewer");

    QAction *openAct = new QAction("Open");
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenMaterialViewer);
    menu->addAction(openAct);

    QAction *closeAct = new QAction("Exit");
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseMaterialViewer);
    menu->addAction(closeAct);

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

  QMenu* WindowsMenu::MakeFileViewerMenu()
  {
    QMenu *menu = new QMenu("File Viewer");

    QAction *openAct = new QAction("Open");
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenFileViewer);
    menu->addAction(openAct);

    QAction *closeAct = new QAction("Exit");
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseFileViewer);
    menu->addAction(closeAct);

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

  QMenu* WindowsMenu::MakeWWiseViewerMenu()
  {
    QMenu *menu = new QMenu("WWise Viewer");

    QAction *openAct = new QAction("Open");
    connect(openAct, &QAction::triggered, this, &WindowsMenu::OpenWWiseViewer);
    menu->addAction(openAct);

    QAction *closeAct = new QAction("Exit");
    connect(closeAct, &QAction::triggered, this, &WindowsMenu::CloseWWiseViewer);
    menu->addAction(closeAct);

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
