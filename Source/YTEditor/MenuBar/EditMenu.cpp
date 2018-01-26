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

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MenuBar/EditMenu.hpp"

#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"

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

    QAction *refreshGizmoAct = new QAction("Refresh Gizmo");
    addAction(refreshGizmoAct);
    connect(refreshGizmoAct, &QAction::triggered, this, &EditMenu::RefreshGizmo);
    refreshGizmoAct->setToolTip("Ctrl+R");
  }

  EditMenu::~EditMenu()
  {
  }

  void EditMenu::RefreshGizmo()
  {
    mMainWindow->GetGizmo()->RefreshAxesInPhysicsHandler();
  }

  void EditMenu::Undo()
  {
  }

  void EditMenu::Redo()
  {
  }

}
