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

#include <qapplication.h>
#include <qfiledialog.h>
#include <qdesktopservices.h>

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Utilities/String/String.h"

#include "../MainWindow/YTEditorMainWindow.hpp"

#include "FileMenu.hpp"


FileMenu::FileMenu(YTEditorMainWindow * aMainWindow)
  : QMenu("File"), mMainWindow(aMainWindow)
{
  QAction * newLevelAct = new QAction("New Level");
  addAction(newLevelAct);
  connect(newLevelAct, &QAction::triggered, this, &FileMenu::NewLevel);

  QAction * openLevelAct = new QAction("Open Level");
  addAction(openLevelAct);
  connect(openLevelAct, &QAction::triggered, this, &FileMenu::OpenLevel);

  QAction * saveLevelAct = new QAction("Save Level");
  addAction(saveLevelAct);
  connect(saveLevelAct, &QAction::triggered, this, &FileMenu::SaveLevel);

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

  mMainWindow->LoadLevel(file_without_extension);
}

void FileMenu::SaveLevel()
{
  auto spaceComp = mMainWindow->GetRunningEngine()->GetCompositions()->begin()->second.get();
  static_cast<YTE::Space*>(spaceComp)->SaveLevel(mMainWindow->GetRunningLevelName());
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
  mMainWindow->GetApplication()->quit();
}