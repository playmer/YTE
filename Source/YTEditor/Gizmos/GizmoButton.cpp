#include <qdir.h>
#include <qevent.h>

#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/Gizmos/GizmoButton.hpp"


namespace YTEditor
{

  GizmoButton::GizmoButton(GizmoToolbar *aToolbar, QString aIconPath, void(GizmoToolbar::*onToggled)(bool))
    : QPushButton(aToolbar)
    , mToolbar(aToolbar)
  {
    setCheckable(true);
    setIconSize(QSize(20, 20));
    setIcon(QIcon(QDir(aIconPath).path()));
    connect(this, &QPushButton::toggled, aToolbar, onToggled);
  }

  void GizmoButton::mousePressEvent(QMouseEvent *event)
  {
    if (this->isChecked())
    {
      event->ignore();
      return;
    }

    auto buttons = mToolbar->GetButtons();

    for (GizmoButton *b : buttons)
    {
      if (b != this)
      {
        b->setChecked(false);
      }
    }

    QPushButton::mousePressEvent(event);
  }
}
