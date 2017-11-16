/******************************************************************************/
/*!
\file   SearchBarEventFilter.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the event filter for the component search bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <iostream>

#include <qabstractitemview.h>
#include <qcompleter.h>
#include <qevent.h>
#include <qlineedit.h>

#include "YTEditor/ComponentBrowser/SearchBarEventFilter.hpp"
#include "YTEditor/ComponentBrowser/ComponentSearchBar.hpp"

namespace YTEditor
{

  SearchBarEventFilter::SearchBarEventFilter(ComponentSearchBar *aLineEdit, QCompleter *aParent)
    : QObject(aParent), mSearchBar(aLineEdit), mCompleter(aParent)
  {
  }

  bool SearchBarEventFilter::eventFilter(QObject * aWatched, QEvent * aEvent)
  {
    QAbstractItemView * view = qobject_cast<QAbstractItemView*>(aWatched);

    if (aEvent->type() == QEvent::KeyPress)
    {
      QKeyEvent * keyEvent = dynamic_cast<QKeyEvent*>(aEvent);

      if (keyEvent->key() == Qt::Key_Return ||
        keyEvent->key() == Qt::Key_Enter)
      {
        mSearchBar->OnReturnPressed();
        mSearchBar->clear();
        view->hide();
        return true;
      }
      else if (keyEvent->key() == Qt::Key_Tab)
      {
        mSearchBar->OnTabPressed();
        return true;
      }
      
    }
    else if (aEvent->type() == QEvent::MouseButtonPress)
    {
      QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent*>(aEvent);

      if (mouseEvent->button() == Qt::MouseButton::LeftButton)
      {
        QPoint mPos = mouseEvent->pos();

        //std::cout << "( " << mPos.x() << ", " << mPos.y() << " )" << std::endl;
        //std::cout << "W: " << mCompleter->popup()->width() << std::endl;
        //std::cout << "H: " << mCompleter->popup()->height() << std::endl << std::endl;

        if (mPos.x() < 0 || mPos.x() > mCompleter->popup()->width() ||
          mPos.y() < 0 || mPos.y() > mCompleter->popup()->height())
        {
          return false;
        }

        mSearchBar->clear();
        view->hide();
        return true;
      }
    }


    return false;
  }

}
