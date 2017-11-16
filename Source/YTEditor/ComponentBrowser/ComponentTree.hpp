/******************************************************************************/
/*!
\file   ComponentTree.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The tree widget containing all components on the current object.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <vector>

#include <qtreewidget.h>

#include "YTEditor/UndoRedo/UndoRedo.hpp"

class QScrollArea;
class QVBoxLayout;
class QGridLayout;

namespace YTE
{
  class Composition;
  class Component;
  class Type;
}


namespace YTEditor
{

  class ComponentBrowser;
  class ComponentWidget;

  class ComponentTree : public QTreeWidget
  {
  public:

    ComponentTree(ComponentBrowser * parent);
    ~ComponentTree();

    void LoadGameObject(YTE::Composition * aObj);

    ComponentWidget * CreateComponent(YTE::Type * aType, const char * aName, YTE::Component * aEngineComp);

    void SetItemToCollapsedColor(QTreeWidgetItem * aItem);

    void SetItemToExpandedColor(QTreeWidgetItem * aItem);

    void AddComponent(ComponentWidget *aWidget);
    void RemoveComponent(QTreeWidgetItem * aWidget);

    // for use by undo/redo system only
    void BaseAddComponent(ComponentWidget * aWidget);
    void BaseRemoveComponent(QTreeWidgetItem * aWidget);

    void ClearComponents();

    std::vector<ComponentWidget*> & GetComponentWidgets();

    void CreateContextMenu(const QPoint & pos);

    void RemoveCurrentComponent();

    ComponentBrowser* GetBrowser();

  private:
    ComponentBrowser * mComponentBrowser;
    UndoRedo *mUndoRedo;
    OutputConsole *mOutputConsole;

    void SetWindowSettings();

    void keyPressEvent(QKeyEvent *aEvent);

    std::vector<ComponentWidget*> mComponentWidgets;
  };
}
