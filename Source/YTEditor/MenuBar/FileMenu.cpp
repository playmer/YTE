/******************************************************************************/
/*!
\file   FileMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the file menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/


#include <filesystem>
#include <fstream>

#include <qapplication>
#include <qfiledialog>
#include <qdesktopservices>
#include <qmessagebox>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/GameWindow/GameWindow.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"

#include "YTEditor/MenuBar/FileMenu.hpp"


namespace YTEditor
{

  FileMenu::FileMenu(MainWindow * aMainWindow)
    : QMenu("File"), mMainWindow(aMainWindow)
  {
    setToolTipsVisible(true);

    QAction * newLevelAct = new QAction("New Level");
    addAction(newLevelAct);
    connect(newLevelAct, &QAction::triggered, this, &FileMenu::NewLevel);

    QAction * openLevelAct = new QAction("Open Level");
    addAction(openLevelAct);
    connect(openLevelAct, &QAction::triggered, this, &FileMenu::OpenLevel);
    //openLevelAct->setToolTip("Ctrl+O");

    QAction * saveLevelAct = new QAction("Save Level");
    addAction(saveLevelAct);
    connect(saveLevelAct, &QAction::triggered, this, &FileMenu::SaveLevel);
    saveLevelAct->setToolTip("Ctrl+S");

    QAction * saveLevelAsAct = new QAction("Save Level As");
    addAction(saveLevelAsAct);
    connect(saveLevelAsAct, &QAction::triggered, this, &FileMenu::SaveLevelAs);
    saveLevelAsAct->setToolTip("Ctrl+Alt+S");

    addSeparator();

    QAction * openFileAct = new QAction("Open File");
    addAction(openFileAct);
    connect(openFileAct, &QAction::triggered, this, &FileMenu::OpenFile);

    addSeparator();

    QAction * exitEditorAct = new QAction("Exit Editor");
    addAction(exitEditorAct);
    connect(exitEditorAct, &QAction::triggered, this, &FileMenu::ExitEditor);
  }

  FileMenu::~FileMenu()
  {
  }

  void FileMenu::NewLevel()
  {
    //namespace fs = std::experimental::filesystem;
    //fs::path workingDir{ YTE::Path::GetGamePath().String() };
    //fs::path assetsDir{ workingDir.parent_path() };
    //
    //QFileDialog dialog;
    //
    //dialog.setWindowFilePath(QString(assetsDir.generic_string().c_str()));
    //dialog.setFileMode(QFileDialog::AnyFile);
    //QString strFile = dialog.getSaveFileName(nullptr, "Create New File");
    //
    //QFile file(strFile);
    //file.open(QIODevice::WriteOnly);
    //
    //
    //file.close();
    //
    //mMainWindow->LoadLevel(strFile.toStdString());

    //YTE::Engine *engine = mMainWindow->GetRunningEngine();
    //
    //// current level name 
    //YTE::String &lvlName = mMainWindow->GetRunningSpaceName();
    //
    //// get the current level
    //YTE::Composition *currLvl = engine->FindFirstCompositionByName(lvlName);
    //
    //engine->RemoveComposition(currLvl);
    //
    //
    //YTE::String newLevelName{ "NewLevel" };
    //
    //// add an empty composition to represent the new level
    //YTE::Space *newLevel = engine->AddComposition<YTE::Space>(newLevelName, engine, nullptr);
    //
    //YTE::String camName{ "Camera" };
    //
    //// add the camera object to the new level
    //YTE::Composition *camera = newLevel->AddComposition<YTE::Composition>(camName, engine, camName, newLevel);
    //
    //// add the camera component to the camera object
    //camera->AddComponent(YTE::Camera::GetStaticType());

    //mMainWindow->GetRunningEngine()->Update();
    //mMainWindow->LoadCurrentLevelInfo();

    mMainWindow->DeleteGizmo();
    mMainWindow->CreateBlankLevel("NewLevel");
    mMainWindow->CreateGizmo(mMainWindow->GetEditingLevel());
  }

  void FileMenu::OpenLevel()
  {
    std::string gamePath = YTE::Path::GetGamePath().String();

    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open Level"), QString(gamePath.c_str()) + "Levels/", tr("Level Files (*.json)"));

    if (fileName == "")
    {
      return;
    }

    // Make QString not crash the program
    QString str1 = fileName;
    QByteArray ba = str1.toLatin1();
    const char *c_str2 = ba.data();

    // Extracting the filename (without extension) from the path
    std::string path(c_str2);
    std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
    size_t p = base_filename.find_last_of('.');
    std::string file_without_extension = base_filename.substr(0, p);
    YTE::String yteFile = YTE::String();
    yteFile = file_without_extension;

    mMainWindow->DeleteGizmo();

    mMainWindow->LoadLevel(file_without_extension);

    auto space = mMainWindow->GetEditingLevel();

    mMainWindow->CreateGizmo(static_cast<YTE::Space*>(space));
  }

  void FileMenu::SaveLevel()
  {
    auto spaceComp = mMainWindow->GetEditingLevel();

    static_cast<YTE::Space*>(spaceComp)->SaveLevel(mMainWindow->GetRunningLevelName());
  }

  void FileMenu::SaveLevelAs()
  {
    std::string gamePath = YTE::Path::GetGamePath().String();

    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Open Level"),
                                                    QString(gamePath.c_str()) + "Levels/",
                                                    tr("Level Files (*.json)"));
    
    if (fileName == "")
    {
      return;
    }

    // Make QString not crash the program
    QString str1 = fileName;
    QByteArray ba = str1.toLatin1();
    const char *c_str2 = ba.data();

    // Extracting the filename (without extension) from the path
    std::string path(c_str2);
    std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
    size_t p = base_filename.find_last_of('.');
    std::string file_without_extension = base_filename.substr(0, p);
    YTE::String yteFile = YTE::String();
    yteFile = file_without_extension;

    auto spaceComp = mMainWindow->GetEditingLevel();

    spaceComp->SetName(yteFile);

    static_cast<YTE::Space*>(spaceComp)->SaveLevel(yteFile);
  }

  void FileMenu::OpenFile()
  {
    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath(), tr("All Files (*.*)"));

    QDesktopServices::openUrl(QUrl("file:///" + fileName, QUrl::TolerantMode));
  }

  void FileMenu::ExitEditor()
  {
    mMainWindow->close();
  }
}