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

  EditMenu::EditMenu(MainWindow * aMainWindow)
    : QMenu("Edit"), mMainWindow(aMainWindow)
  {
    setToolTipsVisible(true);

    QAction *undoAct = new QAction("Undo");
    addAction(undoAct);
    connect(undoAct, &QAction::triggered, this, &EditMenu::Undo);
    undoAct->setToolTip("Ctrl+Z");

    QAction *redoAct = new QAction("Redo");
    addAction(redoAct);
    connect(redoAct, &QAction::triggered, this, &EditMenu::Redo);
    redoAct->setToolTip("Ctrl+Y");
  }

  EditMenu::~EditMenu()
  {
  }

  void EditMenu::Undo()
  {
  }

  void EditMenu::Redo()
  {
  }

}
