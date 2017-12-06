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
#include "YTE/Utilities/Utilities.h"

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

  MainWindow::MainWindow(YTE::Engine * aEngine, QApplication * aQApp)
    : QMainWindow(),
    mRunningEngine(aEngine),
    mApplication(aQApp),
    mObjectBrowser(nullptr),
    mComponentBrowser(nullptr),
    mOutputConsole(nullptr),
    mRunningSpaceName(""),
    mRunningLevelName(""),
    mRunningSpace(nullptr),
    mUndoRedo(new UndoRedo()),
    mGizmo(nullptr),
    mRunningWindow(nullptr),
    mFileMenu(nullptr),
    mGameObjectMenu(nullptr),
    mGizmoScaleFactor(1.0f)
  {
    DebugObjection(!aEngine,
      "Critical Error in YTEditorMainWindow constructor.\n "
      "YTE::Engine *aEngine is nullptr.");

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
    ConstructWWiseWidget();
    LoadCurrentLevelInfo();

    auto self = this;
    QTimer::singleShot(0, [self]()
    {
      self->UpdateEngine();
    });

    CreateGizmo(mEditingLevel);
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

  MaterialViewer& MainWindow::GetMaterialViewer()
  {
    return *static_cast<MaterialViewer*>(mMaterialViewer->widget());
  }

  QDockWidget* MainWindow::GetMaterialViewerDock()
  {
    return mMaterialViewer;
  }

  QTreeView& MainWindow::GetFileViewer()
  {
    return *dynamic_cast<QTreeView*>(mFileViewer->widget());
  }

  QDockWidget* MainWindow::GetFileViewerDock()
  {
    return mFileViewer;
  }

  UndoRedo* MainWindow::GetUndoRedo()
  {
    return mUndoRedo;
  }

  void MainWindow::LoadCurrentLevelInfo()
  {
    // Get all the compositions on the engine
    YTE::CompositionMap *engineMap = mRunningEngine->GetCompositions();

    mRunningEngine->Update();

    // Get all the compositions on the engine
    engineMap = mRunningEngine->GetCompositions();

    // iterator to the main session space
    auto it_lvl = engineMap->begin();

    // Get the space that represents the main session
    YTE::Space * lvl = static_cast<YTE::Space*>(it_lvl->second.get());

    mRunningSpaceName = it_lvl->first;
    mRunningLevelName = lvl->GetLevelName();

    //////////////////////////////////////////////////////////////////////////////
    // Clear the items (names and composition pointers) from the current object browser
    GetObjectBrowser().ClearObjectBrowser();

    // Set the name to the new level
    GetObjectBrowser().setHeaderLabel(lvl->GetName().c_str());

    GetComponentBrowser().GetComponentTree()->ClearComponents();
    /////////////////////////////////////////////////////////////////////////////

    // Get all compositions on the main session (should be levels)
    YTE::CompositionMap * objMap = lvl->GetCompositions();

    // Iterate through all the objects in the map / on the level
    for (auto& cmp : *objMap)
    {
      // Get the name of the object
      YTE::String objName = cmp.second.get()->GetName();

      YTE::Composition *engineObj = cmp.second.get();

      // temp hardcode to not add Gizmo or engineObj to object browser
      if (objName == "Gizmo" || engineObj->GetComponent<YTE::Camera>())
      {
        continue;
      }

      // Store the name and composition pointer in the object browser
      ObjectItem * topItem = this->GetObjectBrowser().AddTreeItem(objName.Data(), cmp.second.get());

      GetObjectBrowser().LoadAllChildObjects(cmp.second.get(), topItem);
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

  YTE::Space * MainWindow::GetMainSession()
  {
    // Get all the compositions on the engine
    YTE::CompositionMap *engineMap = mRunningEngine->GetCompositions();

    // iterator to the main session space
    auto it_mainSession = engineMap->begin();

    mRunningSpaceName = it_mainSession->first;

    // Get the space that represents the main session
    return static_cast<YTE::Space*>(it_mainSession->second.get());
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
    auto mainSession = GetMainSession();
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
    mCentralTabs->addTab(mRunningWindowTab, "Game");

    auto id = mRunningWindow->winId();

    window->SetWindowId(reinterpret_cast<void*>(id));
    renderer->RegisterWindowForDraw(window);

    mRunningSpace->Initialize();
    mRunningSpace->Update(0.0f);
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

    YTE::Space* mainSession = GetMainSession();

    mainSession->CreateBlankLevel(aLevelName);

    mRunningEngine->Update();

    LoadCurrentLevelInfo();
  }

  void MainWindow::LoadLevel(YTE::String aLevelName)
  {
    mRunningLevelName = aLevelName;

    YTE::Space* mainSession = GetMainSession();

    mainSession->LoadLevel(aLevelName);

    mRunningEngine->Update();

    LoadCurrentLevelInfo();
  }

  QApplication* MainWindow::GetApplication()
  {
    return mApplication;
  }

  Gizmo* MainWindow::GetGizmo()
  {
    return mGizmo;
  }

  void MainWindow::keyPressEvent(QKeyEvent * aEvent)
  {
    auto mouse = mLevelWindow->mWindow->mMouse;

    if (aEvent->modifiers() == Qt::Modifier::CTRL)
    {
      if (aEvent->key() == Qt::Key_Z)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Z");
        mUndoRedo->ExecuteUndo();
      }
      else if (aEvent->key() == Qt::Key_Y)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Y");
        mUndoRedo->ExecuteRedo();
      }
      else if (aEvent->key() == Qt::Key_S)
      {
        SaveCurrentLevel();
      }

      if (mouse.IsButtonDown(YTE::Mouse_Buttons::Right) == false)
      {
        if (aEvent->key() == Qt::Key_D)
        {
          mGizmoScaleFactor += 0.02f;
        }
        else if (aEvent->key() == Qt::Key_A)
        {
          mGizmoScaleFactor -= 0.02f;
        }
      }
    }
    else if (aEvent->modifiers() != Qt::Modifier::ALT)
    {
      if (mouse.IsButtonDown(YTE::Mouse_Buttons::Right) == false)
      {
        if (aEvent->key() == Qt::Key_Q)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Select);
        }
        else if (aEvent->key() == Qt::Key_W)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Translate);
        }
        else if (aEvent->key() == Qt::Key_E)
        {
          mGizmoToolbar->SetMode(GizmoToolbar::Mode::Rotate);
        }
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

    mGizmo = new Gizmo(this);
    mGizmo->SetRenderingWindow(yteWin);

    return mGizmo;
  }

  void MainWindow::DeleteGizmo()
  {
    delete mGizmo;
    mGizmo = nullptr;
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
    ConstructGameWindows();
    ConstructObjectBrowser();
    ConstructComponentBrowser();
    ConstructOutputConsole();
    ConstructMaterialViewer();
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

    mGameToolbar = new GameToolbar(this);
    addToolBar(mGameToolbar);
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
    mConsole = new OutputConsole(mOutputConsole);
    mOutputConsole->setWidget(mConsole);
    this->addDockWidget(Qt::BottomDockWidgetArea, mOutputConsole);

  }

  void MainWindow::ConstructMaterialViewer()
  {
    mMaterialViewer = new QDockWidget("Material Viewer", this);
    mObjectBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);


    auto window = mRunningEngine->AddWindow("MaterialViewer");
    MaterialViewer * matViewer = new MaterialViewer(this, mMaterialViewer, window);
    mMaterialViewer->setWidget(matViewer);
    this->addDockWidget(Qt::RightDockWidgetArea, mMaterialViewer);
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

      GetMaterialViewer().GetSubWindow()->mWindow->mEngine = nullptr;
      GetMaterialViewer().GetSubWindow()->mWindow = nullptr;

      GetRunningEngine()->EndExecution();
      GetRunningEngine()->Update();

      event->accept();
    }
    else if (reply == QMessageBox::Discard)
    {
      GetLevelWindow().mWindow->mEngine = nullptr;
      GetLevelWindow().mWindow = nullptr;

      GetMaterialViewer().GetSubWindow()->mWindow->mEngine = nullptr;
      GetMaterialViewer().GetSubWindow()->mWindow = nullptr;

      GetRunningEngine()->EndExecution();
      GetRunningEngine()->Update();

      event->accept();
    }
    else
    {
      event->ignore();
    }
  }

}

