#include <qdir.h>
#include <qevent.h>

#include "YTEditor/YTELevelEditor/Toolbars/Toolbar.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/ToolbarButton.hpp"

namespace YTEditor
{

  ToolbarButton::ToolbarButton(Toolbar *aToolbar, QString aIconPath)
    : QPushButton(aToolbar)
    , mToolbar(aToolbar)
    , mIsResetter(true)
    , mIsResettable(true)
    , mIsUncheckable(false)
  {
    setCheckable(true);
    setIconSize(QSize(20, 20));
    setIcon(QIcon(QDir(aIconPath).path()));
  }

  void ToolbarButton::ResetOtherButtons()
  {
    if (mIsResetter)
    {
      auto buttons = mToolbar->GetButtons();

      for (ToolbarButton *b : buttons)
      {
        if (b != this && b->mIsResettable)
        {
          b->setChecked(false);
        }
      }
    }
  }

  void ToolbarButton::SetResetterMode(bool isResetter)
  {
    mIsResetter = isResetter;
  }

  void ToolbarButton::SetIsUncheckable(bool isUncheckable)
  {
    mIsUncheckable = isUncheckable;
  }

  void ToolbarButton::SetIsResettable(bool isResettable)
  {
    mIsResettable = isResettable;
  }


  void ToolbarButton::mousePressEvent(QMouseEvent *event)
  {
    if (!mIsUncheckable && isCheckable() && isChecked())
    {
      event->ignore();
      return;
    }

    if (mIsUncheckable && !isChecked())
    {
      QPushButton::mousePressEvent(event);
      return;
    }

    if (mIsResetter)
    {
      auto buttons = mToolbar->GetButtons();

      for (ToolbarButton *b : buttons)
      {
        if (b != this && b->mIsResettable)
        {
          b->setChecked(false);
        }
      }
    }
    
    QPushButton::mousePressEvent(event);
  }
}
