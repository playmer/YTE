/******************************************************************************/
/*!
\file   YTEditorYTEditorMainWindow.cpp
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
#include "YTE/StandardLibrary/FileSystem.hpp"
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

#include "crunch/inc/crnlib.h"

#include "stb/stb_image.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

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

#include "YTEditor/YTELevelEditor/Widgets/WWiseViewer/WWiseWidget.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GameToolbar.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

#include "YTEditor/YTELevelEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{
  YTEditorMainWindow::YTEditorMainWindow(YTE::Engine *aEngine, QApplication *aQApp, std::unique_ptr<YTE::RSDocument> aPrefFile)
    : Framework::MainWindow{}
    , mRunningEngine{ aEngine }
    , mApplication{ aQApp }
    , mCentralTabs{ nullptr }
    , mRunningWindowTab{ nullptr }
    , mFileMenu{ nullptr }
    , mGameObjectMenu{ nullptr }
    , mGizmoScaleFactor{ 1.0f }
  {
    DebugObjection(!aEngine,
      "Critical Error in YTEditorMainWindow constructor.\n "
      "YTE::Engine *aEngine is nullptr.");

    LoadPreferences(std::move(aPrefFile));
    SetWindowSettings();

    LoadWorkspace<YTELevelEditor>(this);

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
    auto editor = GetWorkspace<YTELevelEditor>();

    editor->UpdateEngine();

    auto self = this;
    QTimer::singleShot(0, [self]()
    {
      self->UpdateEngine();
    });
  }

  QApplication* YTEditorMainWindow::GetApplication()
  {
    return mApplication;
  }

  void YTEditorMainWindow::keyPressEvent(QKeyEvent * aEvent)
  {
    auto editor = GetWorkspace<YTELevelEditor>();
    auto mouse = editor->GetLevelWindow()->mWindow->mMouse;

    if (aEvent->modifiers() == Qt::Modifier::CTRL)
    {
      // undo
      if (aEvent->key() == Qt::Key_Z)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Z");
        editor->GetUndoRedo()->ExecuteUndo();
      }
      // redo
      else if (aEvent->key() == Qt::Key_Y)
      {
        //GetOutputConsole().PrintLnC(OutputConsole::Color::Green, "Main Window CTRL+Y");
        editor->GetUndoRedo()->ExecuteRedo();
      }
      // save level
      else if (aEvent->key() == Qt::Key_S)
      {
        editor->SaveCurrentLevel();
      }

      if (mouse.IsButtonDown(YTE::MouseButtons::Right) == false)
      {
        // duplicate current object
        if (aEvent->key() == Qt::Key_D)
        {
          GetWorkspace<YTELevelEditor>()->GetWidget<CompositionBrowser>()->DuplicateCurrentlySelected();
        }
      }
    }
    else if (aEvent->modifiers() != Qt::Modifier::ALT)
    {
      if (mouse.IsButtonDown(YTE::MouseButtons::Right) == false)
      {
        // change to select gizmo
        if (aEvent->key() == Qt::Key_Q)
        {
          editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Select);
        }
        // change to translate gizmo
        else if (aEvent->key() == Qt::Key_W)
        {
          editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Translate);
        }
        // change to rotate gizmo
        else if (aEvent->key() == Qt::Key_E)
        {
          editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Rotate);
        }
        // change to scale gizmo
        else if (aEvent->key() == Qt::Key_R)
        {
          editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Scale);
        }
      }
    }
    else
    {
      QMainWindow::keyPressEvent(aEvent);
    }
  }

  FileMenu* YTEditorMainWindow::GetFileMenu()
  {
    return mFileMenu;
  }

  Preferences* YTEditorMainWindow::GetPreferences()
  {
    return &mPreferences;
  }

  // process serialized preferences file
  void YTEditorMainWindow::LoadPreferences(std::unique_ptr<YTE::RSDocument> aPrefFile)
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

  void YTEditorMainWindow::SetWindowSettings()
  {
    // Enables "infinite docking".
    setDockNestingEnabled(true);

    // Sets the default window size.
    resize(1200, 900);
  }

  void YTEditorMainWindow::closeEvent(QCloseEvent *event)
  {
    // ask the user if they want to save the level
    QMessageBox quitConfirm;
    quitConfirm.setWindowTitle("Quit Confirmation");
    quitConfirm.setText("You may have unsaved changes.\nSave your changes before exiting?");
    quitConfirm.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    quitConfirm.setDefaultButton(QMessageBox::Save);

    auto editor = GetWorkspace<YTELevelEditor>();

    int reply = quitConfirm.exec();

    if (reply == QMessageBox::Save)
    {
      editor->SaveCurrentLevel();

      editor->GetLevelWindow()->mWindow->mEngine = nullptr;
      editor->GetLevelWindow()->mWindow = nullptr;

      auto materialViewer = editor->GetWidget<MaterialViewer>();

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
      editor->GetLevelWindow()->mWindow->mEngine = nullptr;
      editor->GetLevelWindow()->mWindow = nullptr;

      auto materialViewer = editor->GetWidget<MaterialViewer>();

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

    std::filesystem::path prefPath{ pref };
    std::filesystem::path baseAndPref = std::filesystem::current_path() / prefPath;

    prefPath = std::filesystem::canonical(baseAndPref);

    YTE::RSStringBuffer sb;
    YTE::RSPrettyWriter writer(sb);
    value.Accept(writer);
    std::string prefInJson = sb.GetString();

    std::ofstream prefToSave;
    prefToSave.open(prefPath);
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

