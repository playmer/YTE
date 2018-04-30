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
#include <qdesktopservices>
#include <qfiledialog>
#include <qmessagebox>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/MainWindow/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/FileMenu.hpp"
#include "YTEditor/MainWindow/SubWindows/GameWindow/GameWindow.hpp"
#include "YTEditor/MainWindow/SubWindows/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MainWindow/SubWindows/OutputConsole/OutputConsole.hpp"


namespace YTEditor
{
  FileMenu::FileMenu(MainWindow *aMainWindow)
    : Menu("File", aMainWindow)
  {
    // add menu options for creating, saving, and opening levels
    AddAction<FileMenu>("New Level", &FileMenu::NewLevel);
    AddAction<FileMenu>("Open Level", &FileMenu::OpenLevel);
    AddAction<FileMenu>("Save Level", &FileMenu::SaveLevel, this, "Ctrl+S");
    AddAction<FileMenu>("Save Level As", &FileMenu::SaveLevelAs, this, "Ctrl+Alt+S");

    addSeparator();

    // menu option for opening a file of any type
    AddAction<FileMenu>("Open File", &FileMenu::OpenFile);

    addSeparator();

    // menu option for closing the editor
    AddAction<FileMenu>("Exit Editor", &FileMenu::ExitEditor);
  }

  void FileMenu::NewLevel()
  {
    // delete the gizmo in the current level
    mMainWindow->DeleteGizmo();

    // create a new empty level
    mMainWindow->CreateBlankLevel("NewLevel");

    // create a new gizmo and add it to the new level
    mMainWindow->CreateGizmo(mMainWindow->GetEditingLevel());
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

    // delete gizmo from the current level
    mMainWindow->DeleteGizmo();

    // load the selected level
    mMainWindow->LoadLevel(file_without_extension);

    // create a new gizmo and add it to the new level
    mMainWindow->CreateGizmo(mMainWindow->GetEditingLevel());
  }

  void FileMenu::SaveLevel()
  {
    // get current level
    YTE::Space *currLevel = mMainWindow->GetEditingLevel();

    // get name of current level
    YTE::String lvlName = mMainWindow->GetRunningLevelName();

    // save current level
    currLevel->SaveLevel(lvlName);
  }

  void FileMenu::SaveLevelAs()
  {
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
    YTE::Space *currLevel = mMainWindow->GetEditingLevel();

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
    // close the editor
    mMainWindow->close();
  }
}