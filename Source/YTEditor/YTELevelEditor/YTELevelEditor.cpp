#include <array>
#include <filesystem>
#include <set>
#include <iostream>
#include <fstream>
#include <thread>

#include <QTimer>
#include <QProgressDialog>
#include <QDockWidget>
#include <QTextEdit>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QFileSystemModel>
#include <QPushButton>
#include <QFileDialog>
#include <QDesktopServices>
#include <QEvent>
#include <QMessageBox>

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

#include "YTEditor/YTELevelEditor/Widgets/YTEWindow/YTEWindow.hpp"
#include "YTEditor/YTELevelEditor/Widgets/YTEWindow/GameWindowEventFilter.hpp"

#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"

#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectItem.hpp"

#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

//#include "YTEditor/YTELevelEditor/Widgets/WWiseViewer/WWiseWidget.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GameToolbar.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  YTELevelEditor::YTELevelEditor(YTEditorMainWindow* aMainWindow)
    : Framework::Workspace(aMainWindow)
    , mRunningEngine{ aMainWindow->GetRunningEngine() }
    , mRunningSpaceName{ "" }
    , mRunningLevelName{ "" }
    , mEditingLevel{ nullptr }
    , mRunningSpace{ nullptr }
    , mEditorCamera{ nullptr }
    , mImguiLayer{ nullptr }
    , mApplication{ nullptr }
    , mUndoRedo{ new UndoRedo{} }
    , mLevelWindow{ nullptr }
    , mLevelWindowWidget{ nullptr }
    , mRunningWindow{ nullptr }
    , mRunningWindowTab{ nullptr }
    , mFileMenu{ nullptr }
    , mGameObjectMenu{ nullptr }
    , mGizmoToolbar{ nullptr }
    , mGameToolbar{ nullptr }
    , mLoading{false}
  {
  }


  bool YTELevelEditor::Initialize()
  {
    // Cstor helper functions and main YTEWindow vars
    ConstructGameWindows();
    ConstructSubWidgets();
    ConstructToolbar();

    // menu bar must be constructed after subwidgets b/c of cached pointers
    ConstructMenuBar();

    // Get all the compositions on the engine
    auto& engineMap = mRunningEngine->GetCompositions();

    // iterator to the main session space
    auto it_lvl = engineMap.begin();

    YTE::Window* yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();

    mEditingLevel = static_cast<YTE::Space*>(it_lvl->second.get());
    mPhysicsHandler = std::make_unique<PhysicsHandler>(mEditingLevel, yteWin, this);

    mRunningEngine->Initialize();
    mEditingLevel->SetIsEditorSpace(mRunningEngine->IsEditor());

    //// This needs to happen after the engine has been initialized.

    AddWidget<MaterialViewer>("Material Viewer", this);
    //AddWidget<WWiseWidget>("WWise Tool", this, mRunningEngine);

    LoadCurrentLevelInfo();

    return true;
  }

  void YTELevelEditor::Shutdown()
  {

  }

  bool YTELevelEditor::KeepLoading()
  {
    if (false == mLoading)
    {
      return false;
    }

    YTE::Space* mainSession = GetEditingLevel();

    if (false == mainSession->GetFinishedLoading())
    {
      mRunningEngine->Update();
      return true;
    }

    LoadCurrentLevelInfo();
    mLoading = false;

    return false;
  }

  void YTELevelEditor::UpdateEngine()
  {
    if (mRunningEngine != nullptr && mRunningEngine->KeepRunning())
    {
      if (KeepLoading())
      {
        return;
      }

      if (mLevelWindow)
      {
        mLevelWindow->Update();
      }

      if (mRunningWindow)
      {
        mRunningWindow->Update();
      }

      mRunningEngine->Update();
    }

    mPhysicsHandler->Update();

    ComponentTree* componentBrowser = GetWidget<ComponentBrowser>()->GetComponentTree();
    std::vector<ComponentWidget*> componentWidgets = componentBrowser->GetComponentWidgets();

    for (auto widget : componentWidgets)
    {
      for (auto property : widget->GetPropertyWidgets())
      {
        property->ReloadValueFromEngine();
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
    CompositionBrowser* objectBrowser = GetWidget<CompositionBrowser>();

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

    // Store the name and composition pointer in the object browser
    objectBrowser->AddTreeItem(lvl->GetName().c_str(), lvl, 0, false);
    auto spaceItem = objectBrowser->topLevelItem(0);
    objectBrowser->setCurrentItem(spaceItem);
    objectBrowser->setItemExpanded(spaceItem, true);

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
    auto window = mRunningEngine->AddWindow("YTEditor Play Window");

    mRunningWindow = new YTEWindow(window, this);
    mRunningWindowTab = QWidget::createWindowContainer(mRunningWindow);
    mRunningWindowTab->setWindowTitle("Level Player");
    mRunningWindowTab->setMinimumSize(480, 320);

    auto toolWindowManager = GetMainWindow()->GetToolWindowManager();

    auto area = toolWindowManager->areaOf(mLevelWindowWidget);
    ToolWindowManager::AreaReference areaToPlace{ 
      ToolWindowManager::AreaReferenceType::AddTo, 
      area
    };

    toolWindowManager->addToolWindow(mRunningWindowTab, areaToPlace);
      //ToolWindowManager::ToolWindowProperty::DisableDraggableTab | 
      //ToolWindowManager::ToolWindowProperty::DisallowFloatWindow | 
      //ToolWindowManager::ToolWindowProperty::DisallowUserDocking);

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
    //mRunningSpace->Initialize(&event);
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

    GetMainWindow()->GetToolWindowManager()->removeToolWindow(mRunningWindowTab);
  }

  void YTELevelEditor::YTEWindowRemoved(YTEWindow* aWindow)
  {
    auto underlyingWindow = aWindow->mWindow;

    if (nullptr != underlyingWindow)
    {
      auto renderer = underlyingWindow->mEngine->GetComponent<YTE::GraphicsSystem>()->GetRenderer();

      if (underlyingWindow == mRunningWindow->mWindow)
      {
        mRunningEngine->RemoveComposition(mRunningSpace);
        mRunningEngine->Update();
      }

      underlyingWindow->mShouldBeRenderedTo = false;
      renderer->DeregisterWindowFromDraw(underlyingWindow);
      mRunningEngine->RemoveWindow(underlyingWindow);
    }

    if (mRunningWindow == aWindow)
    {
      mRunningWindowTab = nullptr;
      mRunningWindow = nullptr;
    }

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
    mLoading = true;
  }

  QApplication* YTELevelEditor::GetApplication()
  {
    return mApplication;
  }
  Gizmo* YTELevelEditor::CreateGizmo(YTE::Space *aSpace)
  {
    YTE::UnusedArguments(aSpace);

    auto gizmo = RemakeGizmo();

    gizmo->SetOperation(Gizmo::Operation::Select);

    return gizmo;
  }

  Gizmo* YTELevelEditor::RemakeGizmo()
  {
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
    AddWidget<CompositionBrowser>("Composition Browser", this);
    AddWidget<ComponentBrowser>("Component Browser", this);
    AddWidget<OutputConsole>("Output Console", this);
    AddWidget<FileViewer>("File Browser", mMainWindow);
  }

  void YTELevelEditor::ConstructGameWindows()
  {
    auto &windows = mRunningEngine->GetWindows();
    auto it = windows.begin();

    mLevelWindow = new YTEWindow(it->second.get(), this);

    auto mainWindow = GetMainWindow();

    GameWindowEventFilter *filter = new GameWindowEventFilter(mLevelWindow, mainWindow);
    mLevelWindow->installEventFilter(filter);

    mLevelWindowWidget = mainWindow->createWindowContainer(mLevelWindow);
    mLevelWindowWidget->setWindowTitle("Level Editor");
    mLevelWindowWidget->setMinimumSize(480, 320);

    // We're disabling movement on this to prevent a crash when you move them around/undock them.
    //
    // Note: The Play Game window and materials window have the same limitation, if you fix this, fix those too.
    mainWindow->GetToolWindowManager()->addToolWindow(
      mLevelWindowWidget,
      ToolWindowManager::EmptySpace
      //ToolWindowManager::ToolWindowProperty::HideCloseButton// |
      //ToolWindowManager::ToolWindowProperty::DisableDraggableTab | 
      //ToolWindowManager::ToolWindowProperty::DisallowFloatWindow | 
      //ToolWindowManager::ToolWindowProperty::DisallowUserDocking
    );

    auto id = mLevelWindow->winId();

    it->second->SetWindowId(reinterpret_cast<void*>(id));
  }

  void YTELevelEditor::ConstructToolbar()
  {
    auto window = static_cast<YTEditorMainWindow*>(mMainWindow);

    mGizmoToolbar = AddToolBar<GizmoToolbar>(GetMainWindow());

    if (!window->GetPreferences()->mNoGameToolbar)
    {
      mGameToolbar = AddToolBar<GameToolbar>(GetMainWindow());
    }
  }

  void YTELevelEditor::ConstructMenuBar()
  {
    //mMenus
    mFileMenu = AddMenu<FileMenu>(mMainWindow);

    AddMenu<EditMenu>(this);
    AddMenu<WindowsMenu>(mMainWindow);

    mGameObjectMenu = AddMenu<GameObjectMenu>(mMainWindow);

    AddMenu<LevelMenu>(mMainWindow);
    AddMenu<ImportMenu>(mMainWindow);
  }

}
