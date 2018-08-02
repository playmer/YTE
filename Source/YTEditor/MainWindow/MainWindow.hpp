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

#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/Physics/PhysicsHandler.hpp"

#include "YTEditor/MainWindow/Widgets/Widget.hpp"

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
  class WWiseWidget;

  class Preferences
  {
  public:
    Preferences();

    void WriteToFile();
    YTE::RSValue Serialize(YTE::RSAllocator &aAllocator);

    void Deserialize(std::unique_ptr<YTE::RSDocument> aPrefFile);

    bool mNoMaterialViewer;
    bool mNoGameToolbar;
  };


  class MainWindow : public QMainWindow
  {
  public:
    MainWindow(YTE::Engine *aEngine, QApplication *aQApp, std::unique_ptr<YTE::RSDocument> aPrefFile = nullptr);
    ~MainWindow();

    template <typename T>
    std::unique_ptr<T> LoadWidget();

    template <typename T>
    T* GetWidget();

  private:

    std::map<std::string, std::unique_ptr<Widget>> mWidgets;


  public:

    UndoRedo* GetUndoRedo();

    void UpdateEngine();

    YTE::Space* GetEditingLevel();

    YTE::Engine* GetRunningEngine() { return mRunningEngine; }

    YTE::String& GetRunningLevelName() { return mRunningLevelName; }

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

    GizmoToolbar* GetGizmoToolbar();

    Preferences* GetPreferences();

    YTE::Composition* GetEditorCamera();

    YTE::Composition* GetImguiLayer()
    {
      return mImguiLayer;
    }

  private:

    Preferences mPreferences;

    // YTE GAME ENGINE
    YTE::Engine *mRunningEngine;
    YTE::String mRunningSpaceName;
    YTE::String mRunningLevelName;

    YTE::Space *mEditingLevel;
    YTE::Space *mRunningSpace;

    YTE::Composition *mEditorCamera;
    YTE::Composition *mImguiLayer;

    QApplication *mApplication;

    UndoRedo *mUndoRedo;

    std::unique_ptr<Gizmo> mGizmo;
    float mGizmoScaleFactor;

    // Cstor helper functions and main subwindow vars
    void LoadPreferences(std::unique_ptr<YTE::RSDocument> aPrefFile);
    void SetWindowSettings();
    void ConstructSubWidgets();
    void ConstructGameWindows();
    void ConstructToolbar();

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



  template <typename T>
  std::unique_ptr<T> MainWindow::LoadWidget()
  {
    // create widget
    std::unique_ptr<T> widget = std::make_unique<T>(this);

    // create dock
    std::unique_ptr<QDockWidget> dock = std::make_unique<QDockWidget>();

    dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    // tell the dock what widget it's holding
    dock->setWidget(widget);

    // store the widget on the main window
    mWidgets.insert_or_assign(widget->GetName(), std::move(widget));

    // add the dock to the main window
    addDockWidget(dock);
  }

  template <typename T>
  T* MainWindow::GetWidget()
  {
    // find the base ptr
    auto it = mWidgets.find(T::GetName());

    if (it == mWidgets.end())
    {
      return nullptr;
    }

    // cast to the derived type
    return static_cast<T*>(it->second.get());
  }

}
