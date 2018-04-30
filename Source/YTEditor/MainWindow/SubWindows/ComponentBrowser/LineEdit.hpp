/******************************************************************************/
/*!
\file   YTELineEdit.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
    A wrapper around the QLineEdit class that filters key presses.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qlineedit.h>
#include <qevent.h>

#include "YTEditor/MainWindow/MainWindow.hpp"

namespace YTEditor
{

  class LineEdit : public QLineEdit
  {
  public:
    LineEdit(QWidget *aParent, MainWindow *aMainWindow)
      : QLineEdit(aParent), mMainWindow(aMainWindow) { }

    ~LineEdit() { }

    void keyPressEvent(QKeyEvent *aEvent)
    {
      this->QLineEdit::keyPressEvent(aEvent);
      return;

      /*
      if (aEvent->modifiers() == Qt::Modifier::CTRL && (aEvent->key() == Qt::Key_Z || aEvent->key() == Qt::Key_Y))
      {
        mMainWindow->keyPressEvent(aEvent);
      }
      else
      {
        this->QLineEdit::keyPressEvent(aEvent);
      }
      */
    }

  private:
    MainWindow *mMainWindow;

  };

}
