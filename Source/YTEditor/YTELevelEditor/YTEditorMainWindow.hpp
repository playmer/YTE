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

#include "YTE/Core/Plugin.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/ForwardDeclarations.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

#include "YTEditor/YTEditorMeta.hpp"

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


  class YTEditorMainWindow : public Framework::MainWindow
  {
  public:
    YTEditor_Shared YTEditorMainWindow(YTE::Engine *aEngine, QApplication *aQApp, std::unique_ptr<YTE::RSDocument> aPrefFile = nullptr);
    YTEditor_Shared ~YTEditorMainWindow();

    YTEditor_Shared void UpdateEngine();

    YTE::Engine* GetRunningEngine() { return mRunningEngine; }

    YTEditor_Shared QApplication* GetApplication();

    YTEditor_Shared void keyPressEvent(QKeyEvent *aEvent);

    YTEditor_Shared FileMenu* GetFileMenu();

    YTEditor_Shared Preferences* GetPreferences();
    
    YTEditor_Shared void LoadPlugins();

  private:
    std::unordered_map<std::string, std::unique_ptr<YTE::PluginWrapper>> mPlugins;
    Preferences mPreferences;

    // YTE GAME ENGINE
    YTE::Engine* mRunningEngine;

    QApplication *mApplication;

    QTabWidget* mCentralTabs;
    QWidget* mRunningWindowTab;

    FileMenu* mFileMenu;
    GameObjectMenu* mGameObjectMenu;

    float mGizmoScaleFactor;

    // Cstor helper functions and main subwindow vars
    void LoadPreferences(std::unique_ptr<YTE::RSDocument> aPrefFile);
    void SetWindowSettings();
    /**************************************************/

    void closeEvent(QCloseEvent *event);
    ////
  };
}
