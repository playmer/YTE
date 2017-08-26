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

#include "YTE/Utilities/String/String.h"

class GameWindow;
class ObjectBrowser;
class ComponentBrowser;
class OutputConsole;
class MaterialViewer;
class QDockWidget;
class QMenu;
class QTabWidget;
class QTreeView;
class QApplication;
class FileMenu;

class UndoRedo;

namespace YTE
{
  class Engine;
  class String;
}

class YTEditorMainWindow : public QMainWindow
{
public:

  YTEditorMainWindow(YTE::Engine * aEngine, QApplication * aQApp);
  ~YTEditorMainWindow();

  GameWindow& GetGameWindow();

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

  UndoRedo * GetUndoRedo();

  void UpdateEngine();

  YTE::Engine* GetRunningEngine() { return mRunningEngine; }

  YTE::String& GetRunningLevelName() { return mRunningLevelName; }

  void LoadLevel(YTE::String aLevelName);
  void LoadCurrentLevelInfo();

  void SaveCurrentLevel();

  QApplication * GetApplication();

  void keyPressEvent(QKeyEvent *aEvent);

private:

  // YTE GAME ENGINE
  YTE::Engine *mRunningEngine;
  YTE::String mRunningLevelName;

  QApplication * mApplication;

  UndoRedo * mUndoRedo;

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
  
  QTabWidget *mCentralTabs;
  GameWindow *mGameWindow;
  GameWindow *mLevelWindow;
  QDockWidget *mObjectBrowser;
  QDockWidget *mComponentBrowser;
  QDockWidget *mOutputConsole;
  QDockWidget *mMaterialViewer;
  QDockWidget *mFileViewer;

  OutputConsole *mConsole;
  /**************************************************/

  // Helper functions for menu bar //
  void ConstructMenuBar();

  FileMenu *mFileMenu;
};