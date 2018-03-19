/******************************************************************************/
/*!
\file   YTEditorMainWindow.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The main window of the application that contains all subwindows and widgets.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

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

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "crunch/inc/crnlib.h"

#include "stb/stb_image.h"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/GameWindow/GameWindow.hpp"
#include "YTEditor/GameWindow/GameWindowEventFilter.hpp"
#include "YTEditor/GameWindow/GameToolbar.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/FileViewer/FileViewer.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MenuBar/EditMenu.hpp"
#include "YTEditor/MenuBar/FileMenu.hpp"
#include "YTEditor/MenuBar/GameObjectMenu.hpp"
#include "YTEditor/MenuBar/ImportMenu.hpp"
#include "YTEditor/MenuBar/LevelMenu.hpp"
#include "YTEditor/MenuBar/WindowsMenu.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/UndoRedo/UndoRedo.hpp"
#include "YTEditor/WWiseViewer/WWiseWidget.hpp"


namespace YTEditor
{
  MainWindow::MainWindow(YTE::Engine *aEngine, QApplication *aQApp, std::unique_ptr<YTE::RSDocument> aPrefFile)
    : QMainWindow()
    , mRunningEngine(aEngine)
    , mApplication(aQApp)
    , mObjectBrowser(nullptr)
    , mComponentBrowser(nullptr)
    , mOutputConsole(nullptr)
    , mRunningSpaceName("")
    , mRunningLevelName("")
    , mRunningSpace(nullptr)
    , mUndoRedo(new UndoRedo())
    , mGizmo(nullptr)
    , mRunningWindow(nullptr)
    , mFileMenu(nullptr)
    , mGameObjectMenu(nullptr)
    , mGizmoScaleFactor(1.0f)
  {
    DebugObjection(!aEngine,
      "Critical Error in YTEditorMainWindow constructor.\n "
      "YTE::Engine *aEngine is nullptr.");

    LoadPreferences(std::move(aPrefFile));
    SetWindowSettings();
    ConstructToolbar();
    ConstructMenuBar();
    ConstructSubWidgets();

    // Get all the compositions on the engine
    auto engineMap = mRunningEngine->GetCompositions();

    // iterator to the main session space
    auto it_lvl = engineMap->begin();

    // get the window
    YTE::Window *yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();

    // Get the space that represents the main session
    mEditingLevel = static_cast<YTE::Space*>(it_lvl->second.get());
    mPhysicsHandler = std::make_unique<PhysicsHandler>(mEditingLevel, yteWin, this);

    aEngine->Initialize();
    mEditingLevel->SetIsEditorSpace(aEngine->IsEditor());

    //// This needs to happen after the engine has been initialized.
    ConstructMaterialViewer();
    ConstructWWiseWidget();

    tabifyDockWidget(mMaterialViewer, mComponentBrowser);
    tabifyDockWidget(mMaterialViewer, mWWiseWidget);

    LoadCurrentLevelInfo();

    CreateGizmo(mEditingLevel);

    auto self = this;
    QTimer::singleShot(0, [self]()
    {
      self->UpdateEngine();
    });
  }

  MainWindow::~MainWindow()
  {
  }

  void MainWindow::UpdateEngine()
  {
    if (mRunningEngine != nullptr)
    {
      mRunningEngine->Update();
    }

    mPhysicsHandler->Update();

    auto self = this;
    QTimer::singleShot(0, [self]()
    {
      self->UpdateEngine();
    });


    std::vector<ComponentWidget*> componentWidgets = GetComponentBrowser().GetComponentTree()->GetComponentWidgets();

    for (ComponentWidget* w : componentWidgets)
    {
      std::vector<PropertyWidgetBase*> properties = w->GetPropertyWidgets();

      for (auto prop : properties)
      {
        prop->ReloadValueFromEngine();
      }
    }

    // scale gizmo w.r.t camera's distance from object
    if (mGizmo)
    {
      auto currentObj = GetObjectBrowser().GetCurrentObject();
      
      if (currentObj)
      {
        auto objTransform = currentObj->GetComponent<YTE::Transform>();
        
        if (objTransform)
        {
          glm::vec3 objPos = objTransform->GetWorldTranslation();

          auto view = mEditingLevel->GetComponent<YTE::GraphicsView>();
          auto cameraComponent = view->GetLastCamera();
          auto cameraObject = cameraComponent->GetOwner();
          auto cameraTransform = cameraObject->GetComponent<YTE::Transform>();
          glm::vec3 camPos = cameraTransform->GetWorldTranslation();

          glm::vec3 dirVec = camPos - objPos;
          float dist = length(dirVec) / 12.0f;

          if (dist < 1.0f)
          {
            dist = 1.0f;
          }

          glm::vec3 gizmoScale = mGizmoScaleFactor * glm::vec3(dist, dist, dist);

          mGizmo->mGizmoObj->GetComponent<YTE::Transform>()->SetScale(gizmoScale);
        }
      }
    }


    //YTE::Composition *currObj = GetObjectBrowser().GetCurrentObject();
    //
    //if (currObj)
    //{
    //  // get the transform of the currently selected object
    //  YTE::Transform *transform = currObj->GetComponent<YTE::Transform>();
    //
    //  if (transform)
    //  {
    //    // set the gizmo to the same position as the current object
    //    glm::vec3 pos = transform->GetWorldTranslation();
    //    YTE::Transform *gizmoTransform = mGizmo->mGizmoObj->GetComponent<YTE::Transform>();
    //    gizmoTransform->SetWorldTranslation(pos);
    //  }
    //}
  }

  YTE::Space* MainWindow::GetEditingLevel()
  {
    return mEditingLevel;
  }

  SubWindow& MainWindow::GetLevelWindow()
  {
    return *mLevelWindow;
  }

  std::vector<SubWindow*>& MainWindow::GetSubWindows()
  {
    return mSubWindows;
  }

  ObjectBrowser& MainWindow::GetObjectBrowser()
  {
    return *static_cast<ObjectBrowser*>(mObjectBrowser->widget());
  }

  QDockWidget* MainWindow::GetObjectBrowserDock()
  {
    return mObjectBrowser;
  }

  ComponentBrowser& MainWindow::GetComponentBrowser()
  {
    return *static_cast<ComponentBrowser*>(mComponentBrowser->widget());
  }

  QDockWidget* MainWindow::GetComponentBrowserDock()
  {
    return mComponentBrowser;
  }

  OutputConsole& MainWindow::GetOutputConsole()
  {
    return *static_cast<OutputConsole*>(mOutputConsole->widget());
  }

  QDockWidget* MainWindow::GetOutputConsoleDock()
  {
    return mOutputConsole;
  }

  MaterialViewer* MainWindow::GetMaterialViewer()
  {
    QWidget *widget = mMaterialViewer->widget();

    if (widget == nullptr)
    {
      return nullptr;
    }

    return static_cast<MaterialViewer*>(widget);
  }

  QDockWidget* MainWindow::GetMaterialViewerDock()
  {
    return mMaterialViewer;
  }

  QTreeView& MainWindow::GetFileViewer()
  {
    return *static_cast<QTreeView*>(mFileViewer->widget());
  }

  QDockWidget* MainWindow::GetFileViewerDock()
  {
    return mFileViewer;
  }

  WWiseWidget& MainWindow::GetWWiseWidget()
  {
    return *static_cast<WWiseWidget*>(mWWiseWidget->widget());
  }

  QDockWidget* MainWindow::GetWWiseWidgetDock()
  {
    return mWWiseWidget;
  }

  UndoRedo* MainWindow::GetUndoRedo()
  {
    return mUndoRedo;
  }

  void MainWindow::LoadCurrentLevelInfo()
  {
    YTE::Space *lvl = GetEditingLevel();

    mRunningSpaceName = lvl->GetName();
    mRunningLevelName = lvl->GetLevelName();

    //////////////////////////////////////////////////////////////////////////////
    // Clear the items (names and composition pointers) from the current object browser
    GetObjectBrowser().ClearObjectBrowser();

    // Set the name to the new level
    GetObjectBrowser().setHeaderLabel(lvl->GetName().c_str());

    GetComponentBrowser().GetComponentTree()->ClearComponents();
    /////////////////////////////////////////////////////////////////////////////

    // Get all compositions on the main session (should be levels)
    YTE::CompositionMap *objMap = lvl->GetCompositions();

    // Iterate through all the objects in the map / on the level
    for (auto cmp = objMap->begin(); cmp != objMap->end(); cmp++)
    {
      // Get the name of the object
      YTE::String objName = cmp->second.get()->GetName();

      YTE::Composition *engineObj = cmp->second.get();

      // temp hardcode to not add Gizmo or engineObj to object browser
      if (objName == "Gizmo" || engineObj->GetComponent<YTE::Camera>())
      {
        continue;
      }

      // Store the name and composition pointer in the object browser
      ObjectItem * topItem = this->GetObjectBrowser().AddTreeItem(objName.Data(), cmp->second.get(), 0, false);

      GetObjectBrowser().LoadAllChildObjects(cmp->second.get(), topItem);
    }

    // if there are objects in the level
    if (objMap->size() != 0)
    {
      GetObjectBrowser().setCurrentItem(GetObjectBrowser().topLevelItem(0));
    }

  }

  void MainWindow::SaveCurrentLevel()
  {
    mFileMenu->SaveLevel();
  }

  void MainWindow::SetRunningSpaceName(YTE::String &aName)
  {
    mRunningSpaceName = aName;
  }

  YTE::String& MainWindow::GetRunningSpaceName()
  {
    return mRunningSpaceName;
  }

  void MainWindow::PlayLevel()
  {
    if (mRunningSpace)
    {
      return;
    }

    YTE::RSAllocator allocator;
    auto mainSession = GetEditingLevel();
    auto value = mainSession->Serialize(allocator);

    mRunningEngine->AddWindow("YTEditor Play Window");
    mRunningSpace = mRunningEngine->AddComposition<YTE::Space>("YTEditor Play Space", mRunningEngine, nullptr);
    mRunningSpace->Load(&value, false);

    auto graphicsView = mRunningSpace->GetComponent<YTE::GraphicsView>();  
    graphicsView->ChangeWindow("YTEditor Play Window");

    auto window = graphicsView->GetWindow();
    window->mShouldBeRenderedTo = true;

    auto renderer = mRunningEngine->GetComponent<YTE::GraphicsSystem>()->GetRenderer();

    mRunningWindow = new SubWindow(window, this);
    mRunningWindowTab = createWindowContainer(mRunningWindow);
    int index = mCentralTabs->addTab(mRunningWindowTab, "Game");
    mCentralTabs->setCurrentIndex(index);

    auto id = mRunningWindow->winId();

    window->SetWindowId(reinterpret_cast<void*>(id));
    renderer->RegisterWindowForDraw(window);

    YTE::LogicUpdate update;
    update.Dt = 0.0f;
    YTE::InitializeEvent event;
    mRunningSpace->Initialize(&event);
    mRunningSpace->Update(&update);
  }

  void MainWindow::PauseLevel(bool pauseState)
  {
    if (mRunningSpace)
    {
      mRunningSpace->SetPaused(pauseState);
    }
  }

  void MainWindow::StopLevel()
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

  void MainWindow::CreateBlankLevel(const YTE::String &aLevelName)
  {
    mRunningLevelName = aLevelName;

    YTE::Space *mainSession = GetEditingLevel();

    mainSession->CreateBlankLevel(aLevelName);

    mRunningEngine->Update();

    LoadCurrentLevelInfo();
  }

  void MainWindow::LoadLevel(YTE::String aLevelName)
  {
    mRunningLevelName = aLevelName;

    YTE::Space* mainSession = GetEditingLevel();

    mainSession->LoadLevel(aLevelName, true);

    mRunningEngine->Update();

    LoadCurrentLevelInfo();
  }

  QApplication* MainWindow::GetApplication()
  {
    return mApplication;
  }

  Gizmo* MainWindow::GetGizmo()
  {
    return mGizmo.get();
  }

  void MainWindow::keyPressEvent(QKeyEvent * aEvent)
  {
    auto mouse = mLevelWindow->mWindow->mMouse;

    if (aEvent->modifiers() == Qt::Modifier::CTRL)
    {
      // undo
      if (aEvent->key() == Qt::Key_Z)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Z");
        mUndoRedo->ExecuteUndo();
      }
      // redo
      else if (aEvent->key() == Qt::Key_Y)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Y");
        mUndoRedo->ExecuteRedo();
      }
      // save level
      else if (aEvent->key() == Qt::Key_S)
      {
        SaveCurrentLevel();
      }
      else if (aEvent->key() == Qt::Key_G)
      {
        GetGizmo()->RefreshAxesInPhysicsHandler();
      }

      if (mouse.IsButtonDown(YTE::Mouse_Buttons::Right) == false)
      {
        // increase gizmo scale factor
        if (aEvent->key() == Qt::Key_E)
        {
          mGizmoScaleFactor += 0.02f;
        }
        // decrease gizmo scale factor
        else if (aEvent->key() == Qt::Key_Q)
        {
          mGizmoScaleFactor -= 0.02f;
        }
        // duplicate current object
        else if (aEvent->key() == Qt::Key_D)
        {
          GetObjectBrowser().DuplicateCurrentlySelected();
        }
      }
    }
    else if (aEvent->modifiers() != Qt::Modifier::ALT)
    {
      if (mouse.IsButtonDown(YTE::Mouse_Buttons::Right) == false)
      {
        // change to select gizmo
        if (aEvent->key() == Qt::Key_Q)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Select);
        }
        // change to translate gizmo
        else if (aEvent->key() == Qt::Key_W)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Translate);
        }
        // change to rotate gizmo
        else if (aEvent->key() == Qt::Key_E)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Rotate);
        }
        // change to scale gizmo
        else if (aEvent->key() == Qt::Key_R)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Scale);
        }
      }
    }
    else
    {
      QMainWindow::keyPressEvent(aEvent);
    }
  }

  FileMenu* MainWindow::GetFileMenu()
  {
    return mFileMenu;
  }

  GameObjectMenu * MainWindow::GetGameObjectMenu()
  {
    return mGameObjectMenu;
  }

  PhysicsHandler& MainWindow::GetPhysicsHandler()
  {
    return *mPhysicsHandler;
  }

  Gizmo* MainWindow::CreateGizmo(YTE::Space *aSpace)
  {
    auto gizmo = RemakeGizmo();

    // get the window
    YTE::Window *yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();
    gizmo->SetRenderingWindow(yteWin);

    gizmo->mGizmoObj = aSpace->AddCompositionAtPosition("Gizmo",
      "Gizmo",
      glm::vec3(0.0f, 0.0f, 0.0f));
    gizmo->SetMode(Gizmo::Select);

    if (gizmo->mGizmoObj->ShouldSerialize())
    {
      gizmo->mGizmoObj->ToggleSerialize();
    }

    return gizmo;
  }

  Gizmo* MainWindow::RemakeGizmo()
  {
    // get the window
    YTE::Window *yteWin = mRunningEngine->GetWindows().at("Yours Truly Engine").get();

    mGizmo = std::make_unique<Gizmo>(this);
    mGizmo->SetRenderingWindow(yteWin);

    return mGizmo.get();
  }

  void MainWindow::DeleteGizmo()
  {
    mGizmo.reset();
  }

  GizmoToolbar* MainWindow::GetGizmoToolbar()
  {
    return mGizmoToolbar;
  }

  Preferences* MainWindow::GetPreferences()
  {
    return &mPreferences;
  }

  // process serialized preferences file
  void MainWindow::LoadPreferences(std::unique_ptr<YTE::RSDocument> aPrefFile)
  {
    // create a default preferences file
    mPreferences = Preferences();
    
    if (aPrefFile)
    {
      mPreferences.Deserialize(std::move(aPrefFile));
    }
    else
    {
      // write out default file
      mPreferences.WriteToFile();
    }
  }

  void MainWindow::SetWindowSettings()
  {
    // Enables "infinite docking".
    this->setDockNestingEnabled(true);

    // Sets the default window size.
    this->resize(1200, 900);
  }

  void MainWindow::ConstructSubWidgets()
  {
    ConstructOutputConsole();
    ConstructGameWindows();
    ConstructObjectBrowser();
    ConstructComponentBrowser();
    ConstructFileViewer();
  }

  void MainWindow::ConstructGameWindows()
  {
    mCentralTabs = new QTabWidget();
    mCentralTabs->setMovable(true);
    mCentralTabs->setTabsClosable(true);
    mCentralTabs->setUsesScrollButtons(true);
    this->setCentralWidget(mCentralTabs);
    
    auto &windows = mRunningEngine->GetWindows();
    auto it = windows.begin();

    mLevelWindow = new SubWindow(it->second.get(), this);

    GameWindowEventFilter *filter = new GameWindowEventFilter(mLevelWindow, this);
    mLevelWindow->installEventFilter(filter);

    auto widget = createWindowContainer(mLevelWindow);
    mCentralTabs->addTab(widget, "Level");

    auto id = mLevelWindow->winId();

    it->second->SetWindowId(reinterpret_cast<void*>(id));

    //for (auto &windowIt : mRunningEngine->GetWindows())
    //{
    //  mSubWindows.push_back(new SubWindow(windowIt.second.get(), this));
    //  auto widget = createWindowContainer(mSubWindows[mSubWindows.size()-1]);
    //  mCentralTabs->addTab(widget, "Level");
    //
    //  auto id = mSubWindows[mSubWindows.size() - 1]->winId();
    //
    //  windowIt.second->SetWindowId(reinterpret_cast<void*>(id));
    //}
  }

  void MainWindow::ConstructToolbar()
  {
    mGizmoToolbar = new GizmoToolbar(this);
    addToolBar(mGizmoToolbar);

    if (!mPreferences.mNoGameToolbar)
    {
      mGameToolbar = new GameToolbar(this);
      addToolBar(mGameToolbar);
    }
  }

  void MainWindow::ConstructObjectBrowser()
  {
    // dockable object browser window
    mObjectBrowser = new QDockWidget("Object Browser", this);
    mObjectBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ObjectBrowser * objBrowser = new ObjectBrowser(this, mObjectBrowser);
    mObjectBrowser->setWidget(objBrowser);
    this->addDockWidget(Qt::LeftDockWidgetArea, mObjectBrowser);
  }


  void MainWindow::ConstructWWiseWidget()
  {
    // dockable component browser window
    mWWiseWidget = new QDockWidget("WWise Bank Browser", this);
    mWWiseWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    WWiseWidget *wwiseWidget = new WWiseWidget(mWWiseWidget, mRunningEngine);
    mWWiseWidget->setWidget(wwiseWidget);
    this->addDockWidget(Qt::RightDockWidgetArea, mWWiseWidget);

    mWWiseWidget->hide();
  }

  void MainWindow::ConstructComponentBrowser()
  {
    // dockable component browser window
    mComponentBrowser = new QDockWidget("Component Browser", this);
    mComponentBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ComponentBrowser * compBrowser = new ComponentBrowser(this, mComponentBrowser);
    mComponentBrowser->setWidget(compBrowser);
    this->addDockWidget(Qt::RightDockWidgetArea, mComponentBrowser);
  }

  void MainWindow::ConstructOutputConsole()
  {
    // dockable output console window
    mOutputConsole = new QDockWidget("Output Console", this);
    mOutputConsole->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    mConsole = new OutputConsole(this, mOutputConsole);
    mOutputConsole->setWidget(mConsole);
    this->addDockWidget(Qt::BottomDockWidgetArea, mOutputConsole);
  }

  void MainWindow::ConstructMaterialViewer()
  {
    mMaterialViewer = new QDockWidget("Material Viewer", this);
    mObjectBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    if (!mPreferences.mNoMaterialViewer)
    {
      auto window = mRunningEngine->AddWindow("MaterialViewer");
      MaterialViewer *matViewer = new MaterialViewer(this, mMaterialViewer, window);
      mMaterialViewer->setWidget(matViewer);
    }

    this->addDockWidget(Qt::RightDockWidgetArea, mMaterialViewer);

    mMaterialViewer->hide();
  }

  void MainWindow::ConstructFileViewer()
  {
    // dockable file browser window
    mFileViewer = new QDockWidget("File Browser", this);
    mFileViewer->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    FileViewer *fileTree = new FileViewer(mFileViewer);
    mFileViewer->setWidget(fileTree);

    this->addDockWidget(Qt::BottomDockWidgetArea, mFileViewer);
  }

  void MainWindow::ConstructMenuBar()
  {
    QMenuBar *menuBar = new QMenuBar(this);

    mFileMenu = new FileMenu(this);
    menuBar->addMenu(mFileMenu);

    menuBar->addMenu(new EditMenu(this));
    menuBar->addMenu(new WindowsMenu(this));

    mGameObjectMenu = new GameObjectMenu(this);
    menuBar->addMenu(mGameObjectMenu);
    
    menuBar->addMenu(new LevelMenu(this));
    menuBar->addMenu(new ImportMenu(this));

    this->setMenuBar(menuBar);
  }

  void MainWindow::closeEvent(QCloseEvent *event)
  {
    // ask the user if they want to save the level
    QMessageBox quitConfirm;
    quitConfirm.setWindowTitle("Quit Confirmation");
    quitConfirm.setText("You may have unsaved changes.\nSave your changes before exiting?");
    quitConfirm.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    quitConfirm.setDefaultButton(QMessageBox::Save);

    int reply = quitConfirm.exec();

    if (reply == QMessageBox::Save)
    {
      SaveCurrentLevel();

      GetLevelWindow().mWindow->mEngine = nullptr;
      GetLevelWindow().mWindow = nullptr;

      auto materialViewer = GetMaterialViewer();

      if (materialViewer)
      {
        materialViewer->GetSubWindow()->mWindow->mEngine = nullptr;
        materialViewer->GetSubWindow()->mWindow = nullptr;
      }

      GetRunningEngine()->EndExecution();
      GetRunningEngine()->Update();

      event->accept();
    }
    else if (reply == QMessageBox::Discard)
    {
      GetLevelWindow().mWindow->mEngine = nullptr;
      GetLevelWindow().mWindow = nullptr;

      auto materialViewer = GetMaterialViewer();

      if (materialViewer)
      {
        materialViewer->GetSubWindow()->mWindow->mEngine = nullptr;
        materialViewer->GetSubWindow()->mWindow = nullptr;
      }

      GetRunningEngine()->EndExecution();
      GetRunningEngine()->Update();

      event->accept();
    }
    else
    {
      event->ignore();
    }
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

