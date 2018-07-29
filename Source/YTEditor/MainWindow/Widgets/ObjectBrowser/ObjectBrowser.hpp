/******************************************************************************/
/*!
\file   ObjectBrowser.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The tree widget containing and displaying all objects in the level.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qtreewidget.h>

#include "YTE/StandardLibrary/Vector.hpp"

#include "YTEditor/MainWindow/Widgets/Widget.hpp"


namespace YTEditor
{
  class ObjectTree;

  class ObjectBrowser : public Widget
  {
  public:
    ObjectBrowser(MainWindow *aMainWindow);
    
    ObjectTree* GetObjectTree() const;

    void ClearObjectList();

  private:
    ObjectTree* mObjectTree;

  };

}
