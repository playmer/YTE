/******************************************************************************/
/*!
\file   YTEditorMainWindow.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
    The main window that contains all other subwindows and widgets.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include <qmainwindow.h>
#include <qevent.h>
#include <qapplication.h>

#include "YTE/Utilities/String/String.h"

#include "YTEditor/Physics/PhysicsHandler.hpp"

class QDockWidget;
class QMenu;
class QTabWidget;
class QTreeView;

namespace YTE
{
  class Engine;
  class String;
}

namespace YTEditor
{
  class ObjectBrowser;
  class ComponentBrowser;
  class OutputConsole;
  class MaterialViewer;
  class SubWindow;
  class FileMenu;
  class UndoRedo;
  class Gizmo;
  class GizmoToolbar;
  class GameToolbar;
  class GameObjectMenu;


  class MainWindow : public QMainWindow
  {
  public:
    MainWindow(YTE::Engine * aEngine, QApplication * aQApp);
    ~MainWindow();

    SubWindow& GetLevelWindow();
    std::vector<SubWindow*>& GetSubWindows();

    ObjectBrowser& GetObjectBrowser();
    QDockWidget* GetObjectBrowserDock();

    ComponentBrowser& GetComponentBrowser();
    QDockWidget* GetComponentBrowserDock();

    OutputConsole& GetOutputConsole();
    QDockWidget* GetOutputConsoleDock();

    MaterialViewer& GetMaterialViewer();
    QDockWidget* GetMaterialViewerDock();

    QTreeView& GetFileViewer();
    QDockWidget* GetFileViewerDock();

    UndoRedo* GetUndoRedo();

    void UpdateEngine();

    YTE::Engine* GetRunningEngine() { return mRunningEngine; }

    YTE::String& GetRunningLevelName() { return mRunningLevelName; }

    YTE::Space* GetMainSession();

    void SetRunningSpaceName(YTE::String &aName);
    YTE::String& GetRunningSpaceName();

    void PlayLevel();
    void PauseLevel(bool pauseState);
    void StopLevel();

    void CreateBlankLevel(const YTE::String& aLevelName);
    void LoadLevel(YTE::String aLevelName);
    void LoadCurrentLevelInfo();

    void SaveCurrentLevel();

    QApplication* GetApplication();

    Gizmo* GetGizmo();

    void keyPressEvent(QKeyEvent *aEvent);

    FileMenu* GetFileMenu();

    GameObjectMenu* GetGameObjectMenu();

    PhysicsHandler& GetPhysicsHandler();

    Gizmo* CreateGizmo(YTE::Space *aSpace);
    Gizmo* RemakeGizmo();
    void DeleteGizmo();

  private:

    // YTE GAME ENGINE
    YTE::Engine *mRunningEngine;
    YTE::String mRunningSpaceName;
    YTE::String mRunningLevelName;

    YTE::Space *mEditingLevel;
    YTE::Space *mRunningSpace;

    QApplication *mApplication;

    UndoRedo *mUndoRedo;

    Gizmo *mGizmo;
    float mGizmoScaleFactor;

    // Cstor helper functions and main subwindow vars //
    void SetWindowSettings();
    void ConstructSubWidgets();
    void ConstructGameWindows();
    void ConstructToolbar();
    void ConstructObjectBrowser();
    void ConstructComponentBrowser();
    void ConstructOutputConsole();
    void ConstructMaterialViewer();
    void ConstructFileViewer();
    void ConstructWWiseWidget();

    std::unique_ptr<PhysicsHandler> mPhysicsHandler;

    QTabWidget *mCentralTabs;
    SubWindow *mLevelWindow;

    QWidget *mRunningWindowTab;
    SubWindow *mRunningWindow;

    std::vector<SubWindow*> mSubWindows;

    QDockWidget *mObjectBrowser;
    QDockWidget *mComponentBrowser;
    QDockWidget *mOutputConsole;
    QDockWidget *mMaterialViewer;
    QDockWidget *mFileViewer;
    QDockWidget *mWWiseWidget;

    OutputConsole *mConsole;
    /**************************************************/

    // Helper functions for menu bar //
    void ConstructMenuBar();

    FileMenu *mFileMenu;
    GameObjectMenu *mGameObjectMenu;

    void closeEvent(QCloseEvent *event);
    ////


    GizmoToolbar *mGizmoToolbar;
    GameToolbar *mGameToolbar;
  };

}
