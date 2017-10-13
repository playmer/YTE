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
#include <qdockWidget.h>
#include <qtextEdit.h>
#include <qmenuBar.h>
#include <qtoolBar.h>
#include <qaction.h>
#include <qfile.h>
#include <qapplication.h>
#include <qdir.h>
#include <qfilesystemmodel.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>
#include <qevent.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "crunch/inc/crnlib.h"

#include "stb/stb_image.h"

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Utilities/Utilities.h"

#include "../WWiseViewer/WWiseWidget.hpp"

#include "../ComponentBrowser/ComponentBrowser.hpp"
#include "../ComponentBrowser/ComponentWidget.hpp"
#include "../ComponentBrowser/PropertyWidget.hpp"

#include "../GameWindow/GameWindow.hpp"

#include "../MaterialViewer/MaterialViewer.hpp"

#include "../MenuBar/FileMenu.hpp"
#include "../MenuBar/EditMenu.hpp"
#include "../MenuBar/WindowsMenu.hpp"
#include "../MenuBar/GameObjectMenu.hpp"
#include "../MenuBar/LevelMenu.hpp"
#include "../MenuBar/ImportMenu.hpp"

#include "../ObjectBrowser/ObjectBrowser.hpp"
#include "../ObjectBrowser/ObjectItem.hpp"

#include "../OutputConsole/OutputConsole.hpp"

#include "../Physics/PhysicsHandler.hpp"

#include "../UndoRedo/UndoRedo.hpp"




#include "YTEditorMainWindow.hpp"

YTEditorMainWindow::YTEditorMainWindow(YTE::Engine * aEngine, QApplication * aQApp)
  : QMainWindow(), 
    mRunningEngine(aEngine),
    mApplication(aQApp), 
    mObjectBrowser(nullptr), 
    mComponentBrowser(nullptr), 
    mOutputConsole(nullptr), 
    mRunningLevelName(""),
    mUndoRedo(new UndoRedo())
{
  DebugObjection(!aEngine, 
              "Critical Error in YTEditorMainWindow constructor.\n "
              "YTE::Engine *aEngine is nullptr.");

  SetWindowSettings();
  ConstructMenuBar();
  ConstructSubWidgets();


  // Get all the compositions on the engine
  auto engineMap = mRunningEngine->GetCompositions();

  // iterator to the main session space
  auto it_lvl = engineMap->begin();

  // Get the space that represents the main session
  YTE::Space * lvl = static_cast<YTE::Space*>(it_lvl->second.get());
  mPhysicsHandler = std::make_unique<PhysicsHandler>(lvl, mRunningEngine->GetWindows().at("Yours Truly Engine").get(), this);

  aEngine->Initialize();
  ConstructWWiseWidget();
  LoadCurrentLevelInfo();

  auto self = this;
  QTimer::singleShot(0, [self]()
  {
    self->UpdateEngine();
  });
}

YTEditorMainWindow::~YTEditorMainWindow()
{
}

void YTEditorMainWindow::UpdateEngine()
{
  if (mRunningEngine != nullptr)
  {
    mRunningEngine->Update();
  }

  auto self = this;
  QTimer::singleShot(0, [self]()
  {
    self->UpdateEngine();
  });
}

GameWindow & YTEditorMainWindow::GetGameWindow()
{
  return *mGameWindow;
}

ObjectBrowser & YTEditorMainWindow::GetObjectBrowser()
{
  return *static_cast<ObjectBrowser*>(mObjectBrowser->widget());
}

QDockWidget * YTEditorMainWindow::GetObjectBrowserDock()
{
  return mObjectBrowser;
}

ComponentBrowser & YTEditorMainWindow::GetComponentBrowser()
{
  return *static_cast<ComponentBrowser*>(mComponentBrowser->widget());
}

QDockWidget * YTEditorMainWindow::GetComponentBrowserDock()
{
  return mComponentBrowser;
}

OutputConsole & YTEditorMainWindow::GetOutputConsole()
{
  return *static_cast<OutputConsole*>(mOutputConsole->widget());
}

QDockWidget * YTEditorMainWindow::GetOutputConsoleDock()
{
  return mOutputConsole;
}

MaterialViewer & YTEditorMainWindow::GetMaterialViewer()
{
  return *static_cast<MaterialViewer*>(mMaterialViewer->widget());
}

QDockWidget * YTEditorMainWindow::GetMaterialViewerDock()
{
  return mMaterialViewer;
}

QTreeView & YTEditorMainWindow::GetFileViewer()
{
  return *dynamic_cast<QTreeView*>(mFileViewer->widget());
}

QDockWidget * YTEditorMainWindow::GetFileViewerDock()
{
  return mFileViewer;
}

UndoRedo * YTEditorMainWindow::GetUndoRedo()
{
  return mUndoRedo;
}

void YTEditorMainWindow::LoadCurrentLevelInfo()
{
  // Get all the compositions on the engine
  YTE::CompositionMap * engineMap = mRunningEngine->GetCompositions();

  mRunningEngine->Update();

  // Get all the compositions on the engine
  engineMap = mRunningEngine->GetCompositions();

  // iterator to the main session space
  auto it_lvl = engineMap->begin();

  // Get the space that represents the main session
  YTE::Space * lvl = static_cast<YTE::Space*>(it_lvl->second.get());

  mRunningLevelName = lvl->GetLevelName();

  //////////////////////////////////////////////////////////////////////////////
  // Clear the items (names and composition pointers) from the current object browser
  GetObjectBrowser().ClearObjectBrowser();

  // Set the name to the new level
  GetObjectBrowser().setHeaderLabel(lvl->GetName().c_str());
  /////////////////////////////////////////////////////////////////////////////

  // Get all compositions on the main session (should be levels)
  YTE::CompositionMap * objMap = lvl->GetCompositions();

  // Iterate through all the objects in the map / on the level
  for (auto& cmp : *objMap)
  {
    // Get the name of the object
    YTE::String objName = cmp.second.get()->GetName();

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

void YTEditorMainWindow::SaveCurrentLevel()
{
    mFileMenu->SaveLevel();
}

void YTEditorMainWindow::LoadLevel(YTE::String aLevelName)
{
  mRunningLevelName = aLevelName;

  // Get all the compositions on the engine
  YTE::CompositionMap * engineMap = mRunningEngine->GetCompositions();

  // iterator to the main session space
  auto it_mainSession = engineMap->begin();

  // Get the space that represents the main session
  YTE::Space * mainSession = static_cast<YTE::Space*>(it_mainSession->second.get());

  mainSession->LoadLevel(aLevelName);

  mRunningEngine->Update();

  LoadCurrentLevelInfo();
}

QApplication * YTEditorMainWindow::GetApplication()
{
  return mApplication;
}

void YTEditorMainWindow::keyPressEvent(QKeyEvent * aEvent)
{
  if (aEvent->modifiers() == Qt::Modifier::CTRL && aEvent->key() == Qt::Key_Z)
  {
    //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Z");
    mUndoRedo->ExecuteUndo();
  }
  else if (aEvent->modifiers() == Qt::Modifier::CTRL && aEvent->key() == Qt::Key_Y)
  {
    //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Y");
    mUndoRedo->ExecuteRedo();
  }
  else
  {
    QMainWindow::keyPressEvent(aEvent);
  }
}

void YTEditorMainWindow::SetWindowSettings()
{
  // Enables "infinite docking".
  this->setDockNestingEnabled(true);

  // Sets the default window size.
  this->resize(1200, 900);
}

void YTEditorMainWindow::ConstructSubWidgets()
{
  ConstructGameWindows();
  ConstructToolbar();
  ConstructObjectBrowser();
  ConstructComponentBrowser();
  ConstructOutputConsole();
  ConstructMaterialViewer();
  ConstructFileViewer();
}

void YTEditorMainWindow::ConstructGameWindows()
{
  mCentralTabs = new QTabWidget(this);
  mCentralTabs->setMovable(true);
  mCentralTabs->setTabsClosable(true);
  mCentralTabs->setUsesScrollButtons(true);
  this->setCentralWidget(mCentralTabs);

  //mGameWindow = new GameWindow(mCentralTabs);
  //mCentralTabs->addTab(mGameWindow, "Game");

  for (auto &windowIt : mRunningEngine->GetWindows())
  {
    auto subWindow = new SubWindow(windowIt.second.get(), this);
    auto widget = createWindowContainer(subWindow);
    mCentralTabs->addTab(widget, "Level");

    auto id = subWindow->winId();

    windowIt.second->SetWindowId(reinterpret_cast<void*>(id));
  }
}

void YTEditorMainWindow::ConstructToolbar()
{
  QToolBar * toolbar = new QToolBar(this);

  QPushButton * select = new QPushButton(toolbar);
  select->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/select.png"));
  select->setIconSize(QSize(20, 20));
  select->setCheckable(true);
  toolbar->addWidget(select);

  QPushButton * translate = new QPushButton(toolbar);
  translate->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/translate.png"));
  translate->setIconSize(QSize(20, 20));
  translate->setCheckable(true);
  toolbar->addWidget(translate);

  QPushButton * rotate = new QPushButton(toolbar);
  rotate->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/rotate.png"));
  rotate->setIconSize(QSize(20, 20));
  rotate->setCheckable(true);
  toolbar->addWidget(rotate);

  QPushButton * scale = new QPushButton(toolbar);
  scale->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/scale.png"));
  scale->setIconSize(QSize(20, 20));
  scale->setCheckable(true);
  toolbar->addWidget(scale);
  this->addToolBar(toolbar);
  
  toolbar = new QToolBar(this);

  QPushButton * play = new QPushButton(toolbar);
  play->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/play.png"));
  play->setIconSize(QSize(20, 20));
  play->setCheckable(true);
  toolbar->addWidget(play);

  QPushButton * pause = new QPushButton(toolbar);
  pause->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/pause.png"));
  pause->setIconSize(QSize(20, 20));
  pause->setCheckable(true);
  toolbar->addWidget(pause);

  QPushButton * stop = new QPushButton(toolbar);
  stop->setIcon(QIcon(QDir::currentPath() + "/../CreativeCommons_Icons/stop.png"));
  stop->setIconSize(QSize(20, 20));
  stop->setCheckable(true);
  toolbar->addWidget(stop);
  this->addToolBar(toolbar);
}

void YTEditorMainWindow::ConstructObjectBrowser()
{
  // dockable object browser window
  mObjectBrowser = new QDockWidget("Object Browser", this);
  mObjectBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  ObjectBrowser * objBrowser = new ObjectBrowser(this, mObjectBrowser);
  mObjectBrowser->setWidget(objBrowser);
  this->addDockWidget(Qt::LeftDockWidgetArea, mObjectBrowser);
}


void YTEditorMainWindow::ConstructWWiseWidget()
{
  // dockable component browser window
  mWWiseWidget = new QDockWidget("WWise Bank Browser", this);
  mWWiseWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  YTE::WWiseWidget *wwiseWidget = new YTE::WWiseWidget(mWWiseWidget, mRunningEngine);
  mWWiseWidget->setWidget(wwiseWidget);
  this->addDockWidget(Qt::RightDockWidgetArea, mWWiseWidget);
}

void YTEditorMainWindow::ConstructComponentBrowser()
{
  // dockable component browser window
  mComponentBrowser = new QDockWidget("Component Browser", this);
  mComponentBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  ComponentBrowser * compBrowser = new ComponentBrowser(this, mComponentBrowser);
  mComponentBrowser->setWidget(compBrowser);
  this->addDockWidget(Qt::RightDockWidgetArea, mComponentBrowser);
}

void YTEditorMainWindow::ConstructOutputConsole()
{
  // dockable output console window
  mOutputConsole = new QDockWidget("Output Console", this);
  mOutputConsole->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  mConsole = new OutputConsole(mOutputConsole);
  mOutputConsole->setWidget(mConsole);
  this->addDockWidget(Qt::BottomDockWidgetArea, mOutputConsole);
  
}

void YTEditorMainWindow::ConstructMaterialViewer()
{
  mMaterialViewer = new QDockWidget("Material Viewer", this);
  mObjectBrowser->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);


  auto window = mRunningEngine->AddWindow("MaterialViewer");
  MaterialViewer * matViewer = new MaterialViewer(this, mMaterialViewer, window);
  mMaterialViewer->setWidget(matViewer);
  this->addDockWidget(Qt::RightDockWidgetArea, mMaterialViewer);
}

void YTEditorMainWindow::ConstructFileViewer()
{
  // dockable file browser window
  mFileViewer = new QDockWidget("File Browser", this);
  mFileViewer->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  
  QFileSystemModel *fileModel = new QFileSystemModel(mFileViewer);
  fileModel->setRootPath(QDir::currentPath() + "/../");
  fileModel->setResolveSymlinks(true);

  QTreeView *tree = new QTreeView();
  tree->setModel(fileModel);
  tree->setRootIndex(fileModel->index(QDir::currentPath() + "/../"));
  mFileViewer->setWidget(tree);

  this->addDockWidget(Qt::BottomDockWidgetArea, mFileViewer);
}

void YTEditorMainWindow::ConstructMenuBar()
{
  QMenuBar *menuBar = new QMenuBar(this);

  mFileMenu = new FileMenu(this);
  menuBar->addMenu(mFileMenu);

  menuBar->addMenu(new EditMenu(this));
  menuBar->addMenu(new WindowsMenu(this));
  menuBar->addMenu(new GameObjectMenu(this));
  menuBar->addMenu(new LevelMenu(this));
  menuBar->addMenu(new ImportMenu(this));

  this->setMenuBar(menuBar);
}
