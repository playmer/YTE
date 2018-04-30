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

#include "YTEditor/MainWindow/MenuBar/Menu.hpp"

namespace YTEditor
{
  class WindowsMenu : public Menu
  {
  public:

    WindowsMenu(MainWindow * aMainWindow);

  private:

    Menu *mObjectBrowserMenu;
    Menu* MakeObjectBrowserMenu();
    void OpenObjectBrowser();
    void CloseObjectBrowser();

    Menu *mComponentBrowserMenu;
    Menu* MakeComponentBrowserMenu();
    void OpenComponentBrowser();
    void CloseComponentBrowser();

    Menu *mOutputConsoleMenu;
    Menu* MakeOutputConsoleMenu();
    void OpenOutputConsole();
    void CloseOutputConsole();

    Menu *mMaterialViewerMenu;
    Menu* MakeMaterialViewerMenu();
    void OpenMaterialViewer();
    void CloseMaterialViewer();

    Menu *mFileViewerMenu;
    Menu* MakeFileViewerMenu();
    void OpenFileViewer();
    void CloseFileViewer();

    Menu *mWWiseViewerMenu;
    Menu* MakeWWiseViewerMenu();
    void OpenWWiseViewer();
    void CloseWWiseViewer();

  };

}
