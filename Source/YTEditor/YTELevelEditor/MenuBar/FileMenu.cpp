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

#include "YTE/StandardLibrary/FileSystem.hpp"
#include <fstream>

#include <QApplication>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/FileMenu.hpp"
#include "YTEditor/YTELevelEditor/Widgets/YTEWindow/YTEWindow.hpp"
#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{
  FileMenu::FileMenu(Framework::MainWindow* aMainWindow)
    : Framework::Menu("File", aMainWindow->GetWorkspace<YTELevelEditor>())
  {
    // add menu options for creating, saving, and opening levels
    AddAction<FileMenu>("New Level", &FileMenu::NewLevel, this);
    AddAction<FileMenu>("Open Level", &FileMenu::OpenLevel, this);
    AddAction<FileMenu>("Save Level", &FileMenu::SaveLevel, this, "Ctrl+S");
    AddAction<FileMenu>("Save Level As", &FileMenu::SaveLevelAs, this, "Ctrl+Alt+S");

    addSeparator();

    // menu option for opening a file of any type
    AddAction<FileMenu>("Open File", &FileMenu::OpenFile, this);

    addSeparator();

    // menu option for closing the editor
    AddAction<FileMenu>("Exit Editor", &FileMenu::ExitEditor, this);
  }

  void FileMenu::NewLevel()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // delete the gizmo in the current level
    levelEditor->DeleteGizmo();

    // create a new empty level
    levelEditor->CreateBlankLevel("NewLevel");

    // create a new gizmo and add it to the new level
    levelEditor->CreateGizmo(levelEditor->GetEditingLevel());
  }

  void FileMenu::OpenLevel()
  {
    // get the path of the assets folder
    std::string gamePath = YTE::Path::GetGamePath().String();

    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open Level"), QString(gamePath.c_str()) + "Levels/", tr("Level Files (*.json)"));

    // make sure the user selected a file
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

    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // delete gizmo from the current level
    levelEditor->DeleteGizmo();

    // load the selected level
    levelEditor->LoadLevel(file_without_extension);
  }

  void FileMenu::SaveLevel()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // get current level
    YTE::Space *currLevel = levelEditor->GetEditingLevel();

    // get name of current level
    YTE::String lvlName = levelEditor->GetRunningLevelName();

    // save current level
    currLevel->SaveLevel(lvlName);
  }

  void FileMenu::SaveLevelAs()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // get path to assets folder
    std::string gamePath = YTE::Path::GetGamePath().String();

    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Open Level"),
                                                    QString(gamePath.c_str()) + "Levels/",
                                                    tr("Level Files (*.json)"));
    
    // make sure the user selected a file
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
    
    // new name of the level
    YTE::String yteFilename = file_without_extension;

    // get current level
    YTE::Space *currLevel = levelEditor->GetEditingLevel();

    // set name of current level
    currLevel->SetName(yteFilename);

    // save current level
    currLevel->SaveLevel(yteFilename);
  }

  void FileMenu::OpenFile()
  {
    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath(), tr("All Files (*.*)"));

    // make sure the user selected a file
    if (fileName == "")
    {
      return;
    }

    // open the selected file using the default program for the user's computer
    QDesktopServices::openUrl(QUrl("file:///" + fileName, QUrl::TolerantMode));
  }

  void FileMenu::ExitEditor()
  {
    auto levelEditor = static_cast<YTELevelEditor*>(mWorkspace);

    // close the editor
    levelEditor->GetMainWindow()->close();
  }
}