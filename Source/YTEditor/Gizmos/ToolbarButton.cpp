#include "..\MainWindow\ToolbarButton.hpp"
#include <qdir.h>
#include <qevent.h>

#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/MainWindow/ToolbarButton.hpp


namespace YTEditor
{

  ToolbarButton::ToolbarButton(Toolbar *aToolbar, QString aIconPath)
    : QPushButton(aToolbar)
    , mToolbar(aToolbar)
  {
    setCheckable(true);
    setIconSize(QSize(20, 20));
    setIcon(QIcon(QDir(aIconPath).path()));
  }

  void ToolbarButton::mousePressEvent(QMouseEvent *event)
  {
    if (this->isChecked())
    {
      event->ignore();
      return;
    }

    auto buttons = mToolbar->GetButtons();

    for (ToolbarButton *b : buttons)
    {
      if (b != this)
      {
        b->setChecked(false);
      }
    }

    QPushButton::mousePressEvent(event);
  }
}
