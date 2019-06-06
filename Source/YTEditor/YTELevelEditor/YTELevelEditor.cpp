#include <array>
#include <filesystem>
#include <set>
#include <iostream>
#include <fstream>
#include <thread>

#include <qtimer.h>
#include <qprogressdialog.h>
#include <qdockwidget.h>
#include <qtextedit.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qfile.h>
#include <qapplication.h>
#include <qdir.h>
#include <qfilesystemmodel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>
#include <qevent.h>
#include <qmessagebox.h>

#include "crunch/inc/crnlib.h"

#include "stb/stb_image.h"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/FlybyCamera.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/ImguiLayer.hpp"
#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/EditMenu.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/FileMenu.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/ImportMenu.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/WindowsMenu.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentProperty.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/PropertyWidget.hpp"

#include "YTEditor/YTELevelEditor/Widgets/FileViewer/FileViewer.hpp"

#include "YTEditor/YTELevelEditor/Widgets/GameWindow/GameWindow.hpp"
#include "YTEditor/YTELevelEditor/Widgets/GameWindow/GameWindowEventFilter.hpp"

#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"

#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

#include "YTEditor/YTELevelEditor/Widgets/WWiseViewer/WWiseWidget.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GameToolbar.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

YTELevelEditor::YTELevelEditor(Framework::MainWindow* mainWindow) 
  : Framework::Workspace(mainWindow)
{

}

void YTELevelEditor::UpdateEngine()
{
  if (mRunningEngine != nullptr && mRunningEngine->KeepRunning())
  {
    mRunningEngine->Update();
  }

  mPhysicsHandler->Update();

  auto self = this;
  QTimer::singleShot(0, [self]()
  {
    self->UpdateEngine();
  });

  ComponentTree* componentBrowser = GetWidget<ComponentBrowser>()->GetComponentTree();
  std::vector<ComponentWidget*> componentWidgets = componentBrowser->GetComponentWidgets();

  for (ComponentWidget* w : componentWidgets)
  {
    std::vector<PropertyWidgetBase*> properties = w->GetPropertyWidgets();

    for (auto prop : properties)
    {
      prop->ReloadValueFromEngine();
    }
  }

}

YTE::Space* YTELevelEditor::GetEditingLevel()
{
  return mEditingLevel;
}

UndoRedo* YTELevelEditor::GetUndoRedo()
{
  return mUndoRedo;
}

void YTELevelEditor::LoadCurrentLevelInfo()
{
  YTE::Space *lvl = GetEditingLevel();
  ObjectBrowser* objectBrowser = GetWidget<ObjectBrowser>();

  mRunningSpaceName = lvl->GetName();
  mRunningLevelName = lvl->GetLevelName();

  //////////////////////////////////////////////////////////////////////////////
  // Clear the items (names and composition pointers) from the current object browser
  objectBrowser->ClearObjectList();

  // Set the name to the new level
  objectBrowser->setHeaderLabel(lvl->GetName().c_str());

  ComponentBrowser* componentBrowser = GetWidget<ComponentBrowser>();
  componentBrowser->GetComponentTree()->ClearComponents();
  /////////////////////////////////////////////////////////////////////////////

  // Add the camera object to the new level
  YTE::String camName{ "EditorCamera" };
  mEditorCamera = mEditingLevel->AddComposition<YTE::Composition>(camName,
    GetRunningEngine(),
    camName,
    mEditingLevel);

  if (mEditorCamera->ShouldSerialize())
  {
    mEditorCamera->ToggleSerialize();
  }

  // add the camera component to the camera object
  mEditorCamera->AddComponent(YTE::Transform::GetStaticType());
  mEditorCamera->AddComponent(YTE::Orientation::GetStaticType());
  mEditorCamera->AddComponent(YTE::Camera::GetStaticType());
  mEditorCamera->AddComponent(YTE::FlybyCamera::GetStaticType());

  mEditorCamera->GetComponent<YTE::Transform>()->SetWorldTranslation({ 0.0f, 0.0f, 5.0f });
  mEditorCamera->GetComponent<YTE::Camera>()->SetCameraAsActive();
  /////////////////////////////////////////////////////////////////////////////

  // Add the imgui layer to the level.
  YTE::String imguiName{ "ImguiEditorLayer" };
  mImguiLayer = mEditingLevel->AddComposition<YTE::Composition>(imguiName,
    GetRunningEngine(),
    imguiName,
    mEditingLevel);

  if (mImguiLayer->ShouldSerialize())
  {
    mImguiLayer->ToggleSerialize();
  }

  

  mImguiLayer->AddComponent(YTE::GraphicsView::GetStaticType());
  auto view = mImguiLayer->GetComponent<YTE::GraphicsView>();
  view->SetOrder(100.f);
  view->SetClearColor(glm::vec4{ 0.f, 0.f, 0.f, 0.f });
  view->ChangeWindow(mLevelWindow->mWindow);

  mImguiLayer->AddComponent(YTE::ImguiLayer::GetStaticType());

  // Get all compositions on the main session (should be levels)
  auto& objMap = lvl->GetCompositions();

  // Iterate through all the objects in the map / on the level
  for (auto cmp = objMap.begin(); cmp != objMap.end(); cmp++)
  {
    // Get the name of the object
    YTE::String objName = cmp->second.get()->GetName();

    YTE::Composition *engineObj = cmp->second.get();

    // Store the name and composition pointer in the object browser
    ObjectItem * topItem = objectBrowser->AddTreeItem(objName.Data(), cmp->second.get(), 0, false);
  }

  // if there are objects in the level
  if (objMap.size() != 0)
  {
    objectBrowser->setCurrentItem(objectBrowser->topLevelItem(0));
  }

  CreateGizmo(mEditingLevel);
}

void YTELevelEditor::SaveCurrentLevel()
{
  mFileMenu->SaveLevel();
}

void YTELevelEditor::SetRunningSpaceName(YTE::String &aName)
{
  mRunningSpaceName = aName;
}

YTE::String& YTELevelEditor::GetRunningSpaceName()
{
  return mRunningSpaceName;
}

void YTELevelEditor::PlayLevel()
{
  if (mRunningSpace)
  {
    return;
  }

  // Make actual "physical" window
  mRunningWindow = new SubWindow(nullptr, this);
  mRunningWindowTab = QWidget::createWindowContainer(mRunningWindow);
  int index = mCentralTabs->addTab(mRunningWindowTab, "Game");
  mCentralTabs->setCurrentIndex(index);

  auto window = mRunningEngine->AddWindow("YTEditor Play Window");

  mRunningWindow->mWindow = window;
  window->mShouldBeRenderedTo = true;
  auto id = mRunningWindow->winId();
  window->SetWindowId(reinterpret_cast<void*>(id));

  auto renderer = mRunningEngine->GetComponent<YTE::GraphicsSystem>()->GetRenderer();
  renderer->RegisterWindowForDraw(window);

  // Serialize the editing level.
  YTE::RSAllocator allocator;
  auto mainSession = GetEditingLevel();
  auto value = mainSession->Serialize(allocator);

  mRunningSpace = mRunningEngine->AddComposition<YTE::Space>("YTEditor Play Space", mRunningEngine, nullptr);
  mRunningSpace->Load(&value, false);

  auto graphicsView = mRunningSpace->GetComponent<YTE::GraphicsView>();
  graphicsView->ChangeWindow("YTEditor Play Window");

  YTE::LogicUpdate update;
  update.Dt = 0.0f;
  YTE::InitializeEvent event;
  mRunningSpace->Initialize(&event);
  mRunningSpace->Update(&update);
}

void YTELevelEditor::PauseLevel(bool pauseState)
{
  if (mRunningSpace)
  {
    mRunningSpace->SetPaused(pauseState);
  }
}

void YTELevelEditor::StopLevel()
{
  if (!mRunningSpace) {
    return;
  }
  auto renderer = mRunningEngine->GetComponent<YTE::GraphicsSystem>()->GetRenderer();
  auto window = mRunningSpace->GetComponent<YTE::GraphicsView>()->GetWindow();

  mRunningEngine->RemoveComposition(mRunningSpace);
  mRunningEngine->Update();

  window->mShouldBeRenderedTo = false;
  renderer->DeregisterWindowFromDraw(window);

  int runningIndex = mCentralTabs->indexOf(mRunningWindowTab);
  mCentralTabs->removeTab(runningIndex);

  delete mRunningWindowTab;
  mRunningWindowTab = nullptr;

  mRunningEngine->RemoveWindow(window);

  mRunningSpace = nullptr;
}

void YTELevelEditor::CreateBlankLevel(const YTE::String &aLevelName)
{
  mRunningLevelName = aLevelName;

  YTE::Space *mainSession = GetEditingLevel();

  mainSession->CreateBlankLevel(aLevelName);

  mRunningEngine->Update();

  LoadCurrentLevelInfo();
}

void YTELevelEditor::LoadLevel(YTE::String aLevelName)
{
  mRunningLevelName = aLevelName;

  YTE::Space* mainSession = GetEditingLevel();

  mainSession->LoadLevel(aLevelName, true);

  mRunningEngine->Update();

  while (false == mainSession->GetFinishedLoading())
  {
    mRunningEngine->Update();
  }

  LoadCurrentLevelInfo();
}

QApplication* YTELevelEditor::GetApplication()
{
  return mApplication;
}
Gizmo* YTELevelEditor::CreateGizmo(YTE::Space *aSpace)
{
  auto gizmo = RemakeGizmo();

  // get the window 
  YTE::Window *yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();
  gizmo->SetOperation(Gizmo::Operation::Select);

  return gizmo;
}

Gizmo* YTELevelEditor::RemakeGizmo()
{
  // get the window 
  YTE::Window *yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();

  mGizmo = std::make_unique<Gizmo>(this,
    mImguiLayer->GetComponent<YTE::ImguiLayer>(),
    mEditorCamera->GetComponent<YTE::Camera>());

  return mGizmo.get();
}

void YTELevelEditor::DeleteGizmo()
{
  mGizmo.reset();
}

Gizmo* YTELevelEditor::GetGizmo()
{
  return mGizmo.get();
}

FileMenu* YTELevelEditor::GetFileMenu()
{
  return mFileMenu;
}

GameObjectMenu* YTELevelEditor::GetGameObjectMenu()
{
  return mGameObjectMenu;
}

PhysicsHandler* YTELevelEditor::GetPhysicsHandler()
{
  return mPhysicsHandler.get();
}


GizmoToolbar* YTELevelEditor::GetGizmoToolbar()
{
  return mGizmoToolbar;
}

YTE::Composition * YTELevelEditor::GetEditorCamera()
{
  return mEditorCamera;
}

void YTELevelEditor::ConstructSubWidgets()
{
  AddWidget<ObjectBrowser>();
  AddWidget<ComponentBrowser>();
  AddWidget<OutputConsole>();
  AddWidget<MaterialViewer>();
  AddWidget<FileViewer>();
  AddWidget<WWiseWidget>();

  // Game Windows
  ConstructGameWindows();
}

void YTELevelEditor::ConstructGameWindows()
{
  mCentralTabs = new QTabWidget();
  mCentralTabs->setMovable(true);
  mCentralTabs->setTabsClosable(true);
  mCentralTabs->setUsesScrollButtons(true);
  GetMainWindow()->setCentralWidget(mCentralTabs);

  auto &windows = mRunningEngine->GetWindows();
  auto it = windows.begin();

  mLevelWindow = new SubWindow(it->second.get(), this);

  GameWindowEventFilter *filter = new GameWindowEventFilter(mLevelWindow, GetMainWindow());
  mLevelWindow->installEventFilter(filter);

  auto widget = GetMainWindow()->createWindowContainer(mLevelWindow);
  mCentralTabs->addTab(widget, "Level");

  auto id = mLevelWindow->winId();

  it->second->SetWindowId(reinterpret_cast<void*>(id));
}

void YTELevelEditor::ConstructToolbar()
{
  auto window = static_cast<YTEditorMainWindow*>(mMainWindow);

  mGizmoToolbar = AddToolBar<GizmoToolbar>(this);

  if (!window->GetPreferences()->mNoGameToolbar)
  {
    mGameToolbar = AddToolBar<GameToolbar>(this);
  }
}

void YTELevelEditor::ConstructMenuBar()
{
  mFileMenu = AddMenu<FileMenu>(this);

  AddMenu<EditMenu>(this);
  AddMenu<WindowsMenu>(this);

  mGameObjectMenu = AddMenu<GameObjectMenu>(this);

  AddMenu<LevelMenu>(this);
  AddMenu<ImportMenu>(this);
}

Preferences::Preferences()
  : mNoMaterialViewer(false)
  , mNoGameToolbar(false)
{
}

void Preferences::WriteToFile()
{
  YTE::RSAllocator allocator;
  auto value = Serialize(allocator);

  std::string prefName{ "Preferences" };
  std::wstring pref{ prefName.begin(), prefName.end() };

  std::string path = YTE::Path::GetGamePath().String();
  std::wstring pathWStr{ path.begin(), path.end() };

  pref = pathWStr + pref + L".json";

  pref = std::experimental::filesystem::canonical(pref, std::experimental::filesystem::current_path());

  YTE::RSStringBuffer sb;
  YTE::RSPrettyWriter writer(sb);
  value.Accept(writer);
  std::string prefInJson = sb.GetString();

  std::ofstream prefToSave;
  prefToSave.open(pref);
  prefToSave << prefInJson;
  prefToSave.close();
}

YTE::RSValue Preferences::Serialize(YTE::RSAllocator &aAllocator)
{
  YTE::RSValue toReturn;
  toReturn.SetObject();

  YTE::RSValue noMatViewer;
  noMatViewer.SetBool(mNoMaterialViewer);
  toReturn.AddMember("NoMaterialViewer", mNoMaterialViewer, aAllocator);

  YTE::RSValue noGameToolbar;
  noGameToolbar.SetBool(mNoGameToolbar);
  toReturn.AddMember("NoGameToolbar", mNoGameToolbar, aAllocator);

  return toReturn;
}

void Preferences::Deserialize(std::unique_ptr<YTE::RSDocument> aPrefFile)
{
  if (aPrefFile->HasMember("NoMaterialViewer"))
  {
    mNoMaterialViewer = (*aPrefFile)["NoMaterialViewer"].GetBool();
  }

  if (aPrefFile->HasMember("NoGameToolbar"))
  {
    mNoGameToolbar = (*aPrefFile)["NoGameToolbar"].GetBool();
  }
}

}
