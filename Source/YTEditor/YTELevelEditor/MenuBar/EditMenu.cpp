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

#include "YTEditor/MainWindow/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/MenuBar/EditMenu.hpp"
#include "YTEditor/MainWindow/Toolbars/GizmoToolbar.hpp"

namespace YTEditor
{

  EditMenu::EditMenu(MainWindow *aMainWindow)
    : Menu("Edit", aMainWindow)
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
