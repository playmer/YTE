#pragma once

#include "YTEditor/Framework/Workspace.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

namespace YTEditor
{
  class YTELevelEditor : public Framework::Workspace
  {
  public:
    YTELevelEditor(YTEditorMainWindow* aMainWindow);

    bool Initialize() override;
    void Shutdown() override;

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

    void PostEngineInitilization();

    QApplication* GetApplication();

    Gizmo* GetGizmo();

    FileMenu* GetFileMenu();

    GameObjectMenu* GetGameObjectMenu();

    PhysicsHandler* GetPhysicsHandler();

    Gizmo* CreateGizmo(YTE::Space *aSpace);
    Gizmo* RemakeGizmo();
    void DeleteGizmo();

    GizmoToolbar* GetGizmoToolbar();

    YTE::Composition* GetEditorCamera();

    YTE::Composition* GetImguiLayer()
    {
      return mImguiLayer;
    }

    YTEWindow* GetLevelWindow()
    {
      return mLevelWindow;
    }

    QWidget* GetLevelWindowWidget()
    {
      return mLevelWindowWidget;
    }

  private:
    // YTE GAME ENGINE
    YTE::Engine* mRunningEngine;
    YTE::String mRunningSpaceName;
    YTE::String mRunningLevelName;

    YTE::Space* mEditingLevel;
    YTE::Space* mRunningSpace;

    YTE::Composition* mEditorCamera;
    YTE::Composition* mImguiLayer;

    QApplication *mApplication;

    UndoRedo* mUndoRedo;

    YTEWindow* mLevelWindow;
    QWidget* mLevelWindowWidget;

    YTEWindow* mRunningWindow;
    QWidget* mRunningWindowTab;

    FileMenu* mFileMenu;
    GameObjectMenu* mGameObjectMenu;

    GizmoToolbar* mGizmoToolbar;
    GameToolbar* mGameToolbar;

    std::unique_ptr<Gizmo> mGizmo;
    float mGizmoScaleFactor;
    bool mLoading;

    // Cstor helper functions and main subwindow vars
    void ConstructSubWidgets();
    void ConstructGameWindows();
    void ConstructToolbar();

    std::unique_ptr<PhysicsHandler> mPhysicsHandler;
    std::vector<YTEWindow*> mSubWindows;

    /**************************************************/

    // Helper functions for menu bar //
    void ConstructMenuBar();
    bool KeepLoading();


    void closeEvent(QCloseEvent *event);
    ////

  };
}

