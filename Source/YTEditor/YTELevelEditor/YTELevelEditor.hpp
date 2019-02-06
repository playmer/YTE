#include "YTEditor/Framework/Workspace.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"

namespace YTEditor
{

class UndoRedo;
class FileMenu;
class GameObjectMenu;

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

class YTELevelEditor : public Framework::Workspace
{
public:
  YTELevelEditor(Framework::MainWindow* mainWindow);

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
  void ConstructSubWidgets();
  void ConstructGameWindows();
  void ConstructToolbar();

  std::unique_ptr<PhysicsHandler> mPhysicsHandler;

  QTabWidget *mCentralTabs;
  SubWindow *mLevelWindow;

  QWidget *mRunningWindowTab;
  SubWindow *mRunningWindow;

  std::vector<SubWindow*> mSubWindows;
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

