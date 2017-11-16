/******************************************************************************/
/*!
\file   WindowsMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The drop down on the main window menu bar that controls opening and
closing subwindows/widgets.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qmenu.h>

namespace YTEditor
{

  class MainWindow;

  class WindowsMenu : public QMenu
  {
  public:

    WindowsMenu(MainWindow * aMainWindow);
    ~WindowsMenu();

  private:

    MainWindow * mMainWindow;

    QMenu * mObjectBrowserMenu;
    QMenu * MakeObjectBrowserMenu();
    void OpenObjectBrowser();
    void CloseObjectBrowser();

    QMenu * mComponentBrowserMenu;
    QMenu * MakeComponentBrowserMenu();
    void OpenComponentBrowser();
    void CloseComponentBrowser();

    QMenu * mOutputConsoleMenu;
    QMenu * MakeOutputConsoleMenu();
    void OpenOutputConsole();
    void CloseOutputConsole();

    QMenu * mMaterialViewerMenu;
    QMenu * MakeMaterialViewerMenu();
    void OpenMaterialViewer();
    void CloseMaterialViewer();

    QMenu * mFileViewerMenu;
    QMenu * MakeFileViewerMenu();
    void OpenFileViewer();
    void CloseFileViewer();

  };

}
