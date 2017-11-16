/******************************************************************************/
/*!
\file   SearchBarEventFilter.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The class that handles key events for the component search bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qobject.h>

class QCompleter;

namespace YTEditor
{

  class ComponentSearchBar;

  class SearchBarEventFilter : public QObject
  {
  public:

    SearchBarEventFilter(ComponentSearchBar *aLineEdit, QCompleter *aParent);

    virtual ~SearchBarEventFilter() { }

    bool eventFilter(QObject *aWatched, QEvent * aEvent);

  private:

    ComponentSearchBar * mSearchBar;
    QCompleter * mCompleter;

  };

}
