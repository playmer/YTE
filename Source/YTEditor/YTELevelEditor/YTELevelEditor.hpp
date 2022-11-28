#pragma once

#include "YTEditor/Framework/Workspace.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

#include "YTEditor/YTEditorMeta.hpp"

namespace YTEditor
{
  class YTELevelEditor : public Framework::Workspace
  {
  public:
    YTEditor_Shared YTELevelEditor(YTEditorMainWindow* aMainWindow);

    YTEditor_Shared bool Initialize() override;
    YTEditor_Shared void Shutdown() override;

    YTEditor_Shared UndoRedo* GetUndoRedo();

    YTEditor_Shared void UpdateEngine();

    YTEditor_Shared YTE::Space* GetEditingLevel();

    YTE::Engine* GetRunningEngine() { return mRunningEngine; }

    YTE::String& GetRunningLevelName() { return mRunningLevelName; }

    YTEditor_Shared void SetRunningSpaceName(YTE::String &aName);
    YTEditor_Shared YTE::String& GetRunningSpaceName();

    YTEditor_Shared void PlayLevel();
    YTEditor_Shared void PauseLevel(bool pauseState);
    YTEditor_Shared void StopLevel();

    YTEditor_Shared void CreateBlankLevel(const YTE::String& aLevelName);
    YTEditor_Shared void LoadLevel(YTE::String aLevelName);
    YTEditor_Shared void LoadCurrentLevelInfo();

    YTEditor_Shared void SaveCurrentLevel();
    
    YTEditor_Shared QApplication* GetApplication();

    YTEditor_Shared Gizmo* GetGizmo();

    YTEditor_Shared FileMenu* GetFileMenu();

    YTEditor_Shared GameObjectMenu* GetGameObjectMenu();

    YTEditor_Shared PhysicsHandler* GetPhysicsHandler();

    YTEditor_Shared Gizmo* CreateGizmo(YTE::Space *aSpace);
    YTEditor_Shared Gizmo* RemakeGizmo();
    YTEditor_Shared void DeleteGizmo();

    YTEditor_Shared GizmoToolbar* GetGizmoToolbar();

    YTEditor_Shared YTE::Composition* GetEditorCamera();

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

    bool IsLoading()
    {
        return mLoading;
    }

    // This is so the YTEWindow can report if it's being removed.
    void YTEWindowRemoved(YTEWindow* aWindow);

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

