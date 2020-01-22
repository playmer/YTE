#include <qdir.h>
#include <qevent.h>

#include "YTEditor/Framework/ToolBar.hpp"
#include "YTEditor/Framework/ToolBarButton.hpp"

namespace YTEditor
{
  namespace Framework
  {
    ToolBarButton::ToolBarButton(ToolBar* aToolbar, QString aIconPath, QString aActionName)
      : QPushButton(aToolbar)
      , mIconPath{ aIconPath }
      , mActionName{ aActionName }
      , mToolbar{ aToolbar }
      , mIsResetter{ true }
      , mIsResettable{ true }
      , mIsUncheckable{ false }
    {
      setCheckable(true);
      setIconSize(QSize(20, 20));
      setIcon(QIcon(QDir(aIconPath).path()));
    }

    void ToolBarButton::ResetOtherButtons()
    {
      if (mIsResetter)
      {
        auto buttons = mToolbar->GetButtons();

        for (ToolBarButton* b : buttons)
        {
          if (b != this && b->mIsResettable)
          {
            b->setChecked(false);
          }
        }
      }
    }

    void ToolBarButton::SetResetterMode(bool isResetter)
    {
      mIsResetter = isResetter;
    }

    void ToolBarButton::SetIsUncheckable(bool isUncheckable)
    {
      mIsUncheckable = isUncheckable;
    }

    void ToolBarButton::SetIsResettable(bool isResettable)
    {
      mIsResettable = isResettable;
    }


    void ToolBarButton::mousePressEvent(QMouseEvent* event)
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

        for (ToolBarButton* b : buttons)
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
}
