/******************************************************************************/
/*!
\file   EditMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the edit menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "EditMenu.hpp"

#include "YTEditorMainWindow.hpp"

EditMenu::EditMenu(YTEditorMainWindow * aMainWindow) 
  : QMenu("Edit"), mMainWindow(aMainWindow)
{
  addAction(new QAction("Placeholder"));
}

EditMenu::~EditMenu()
{
}
