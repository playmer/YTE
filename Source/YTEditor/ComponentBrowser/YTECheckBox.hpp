/******************************************************************************/
/*!
\file   YTECheckBox.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
A wrapper around QCheckBox that filters key presses.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qcheckbox.h>
#include <qevent.h>

#include "../MainWindow/YTEditorMainWindow.hpp"

class YTECheckBox : public QCheckBox
{
public:
  YTECheckBox(QWidget *aParent, YTEditorMainWindow *aMainWindow) : QCheckBox(aParent), mMainWindow(aMainWindow) { }

  ~YTECheckBox() { }

  void keyPressEvent(QKeyEvent *aEvent)
  {
    this->QCheckBox::keyPressEvent(aEvent);
    return;

    /*
    if (aEvent->modifiers() == Qt::Modifier::CTRL && (aEvent->key() == Qt::Key_Z || aEvent->key() == Qt::Key_Y))
    {
      mMainWindow->keyPressEvent(aEvent);
    }
    else
    {
      this->QCheckBox::keyPressEvent(aEvent);
    }
    */
  }
  

private:
  YTEditorMainWindow *mMainWindow;

};