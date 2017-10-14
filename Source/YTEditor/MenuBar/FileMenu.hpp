/******************************************************************************/
/*!
\file   FileMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for creating, opening, and saving files.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qmenu.h>

class YTEditorMainWindow;

class FileMenu : public QMenu
{
public:

  FileMenu(YTEditorMainWindow *aMainWindow);
  ~FileMenu();
  void SaveLevel();

private:

  YTEditorMainWindow * mMainWindow;

  void NewLevel();
  void OpenLevel();
  void OpenFile();
  void ExitEditor();


};