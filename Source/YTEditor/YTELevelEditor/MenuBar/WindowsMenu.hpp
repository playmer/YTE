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

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/Menu.hpp"

namespace YTEditor
{
  class WindowsMenu : public Framework::Menu
  {
  public:

    WindowsMenu(Framework::MainWindow* aMainWindow);

  private:
    Framework::Menu* MakeObjectBrowserMenu();
    void OpenObjectBrowser();
    void CloseObjectBrowser();

    Framework::Menu* MakeComponentBrowserMenu();
    void OpenComponentBrowser();
    void CloseComponentBrowser();

    Framework::Menu* MakeOutputConsoleMenu();
    void OpenOutputConsole();
    void CloseOutputConsole();

    Framework::Menu* MakeMaterialViewerMenu();
    void OpenMaterialViewer();
    void CloseMaterialViewer();

    Framework::Menu* MakeFileViewerMenu();
    void OpenFileViewer();
    void CloseFileViewer();

    Framework::Menu* MakeWWiseViewerMenu();
    void OpenWWiseViewer();
    void CloseWWiseViewer();

    Framework::Menu* mObjectBrowserMenu;
    Framework::Menu* mComponentBrowserMenu;
    Framework::Menu* mOutputConsoleMenu;
    Framework::Menu* mMaterialViewerMenu;
    Framework::Menu* mFileViewerMenu;
    Framework::Menu* mWWiseViewerMenu;
  };

}
