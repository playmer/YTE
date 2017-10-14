/******************************************************************************/
/*!
\file   ImportMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for importing models.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qmenu.h>

class YTEditorMainWindow;

class ImportMenu : public QMenu
{
public:

  ImportMenu(YTEditorMainWindow *aMainWindow);
  ~ImportMenu();

private:

  void ImportModel();

  YTEditorMainWindow * mMainWindow;

};