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

#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

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

    ComponentTree(ComponentBrowser *parent);
    ~ComponentTree();

    void LoadGameObject(YTE::Composition *aObj);

    ComponentWidget* CreateComponent(YTE::Type *aType, 
                                     const char *aName,
                                     YTE::Component *aEngineComp,
                                     QTreeWidgetItem *aTopItem);

    void SetItemToCollapsedColor(QTreeWidgetItem *aItem);

    void SetItemToExpandedColor(QTreeWidgetItem *aItem);

    QTreeWidgetItem* FindComponentItem(YTE::Type *aComponentType);

    ComponentWidget* AddComponent(YTE::Type *aComponentType);
    void RemoveComponent(QTreeWidgetItem *aWidget);

    // for use by undo/redo system only
    ComponentWidget* InternalAddComponent(YTE::Type *aComponentType);
    ComponentWidget* InternalAddComponent(YTE::Type *aComponentType, YTE::RSValue *aSerializedComponent);
    void BaseAddComponent(ComponentWidget *aWidget, QTreeWidgetItem *aTopItem);
    void BaseRemoveComponent(QTreeWidgetItem *aWidget);

    void ClearComponents();

    std::vector<ComponentWidget*>& GetComponentWidgets();

    void CreateContextMenu(const QPoint &pos);

    void RemoveCurrentTreeItem();

    void RemoveCurrentComponent();

    void RemoveCurrentHeaderListWidget();

    ComponentBrowser* GetBrowser();

  private:
    ComponentBrowser *mComponentBrowser;
    UndoRedo *mUndoRedo;
    OutputConsole *mOutputConsole;

    //Qt::ItemFlags flags(const QModelIndex &index) const override;

    void SetWindowSettings();

    void keyPressEvent(QKeyEvent *aEvent) override;

    void dropEvent(QDropEvent *aEvent) override;

    void dragEnterEvent(QDragEnterEvent *aEvent) override;

    void dragMoveEvent(QDragMoveEvent *aEvent) override;

    std::vector<ComponentWidget*> mComponentWidgets;
  };
}
