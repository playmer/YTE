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

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/MainWindow.hpp"
#include "YTEditor/YTELevelEditor/MenuBar/EditMenu.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

namespace YTEditor
{

  EditMenu::EditMenu(MainWindow *aMainWindow)
    : Framework::Menu("Edit", aMainWindow)
  {
    AddAction<EditMenu>("Undo", &EditMenu::Undo, this, "Ctrl+Z");
    AddAction<EditMenu>("Redo", &EditMenu::Redo, this, "Ctrl+Y");
  }

  void EditMenu::Undo()
  {
  }

  void EditMenu::Redo()
  {
  }

}
