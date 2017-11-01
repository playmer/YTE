/******************************************************************************/
/*!
\file   ComponentTree.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the ComponentTree that holds ComponentWidgets.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <qdockwidget.h>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qaction.h>
#include <qmenu.h>
#include <qevent.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Meta/Type.hpp"
#include "YTE/Utilities/JsonHelpers.h"

#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/UndoRedo/UndoRedo.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"

namespace YTEditor
{

  ComponentTree::ComponentTree(ComponentBrowser * parent)
    : QTreeWidget(parent),
    mComponentBrowser(parent),
    mUndoRedo(mComponentBrowser->GetMainWindow()->GetUndoRedo())
  {
    SetWindowSettings();

    connect(this,
      &QTreeWidget::customContextMenuRequested,
      this,
      &ComponentTree::CreateContextMenu);

    setItemDelegate(new ComponentDelegate(this));
  }

  ComponentTree::~ComponentTree()
  {
  }

  void ComponentTree::LoadGameObject(YTE::Composition * aObj)
  {
    this->ClearComponents();

    // Load all of the components on the object into the browser
    YTE::ComponentMap & components = *(aObj->GetComponents());

    for (auto& comp : components)
    {
      YTE::String name = comp.first->GetName();
      ComponentWidget * widg = CreateComponent(comp.second.get()->GetType(),
        name.c_str(),
        comp.second.get());
      widg->LoadProperties(*comp.second.get());
      BaseAddComponent(widg);
    }

    QString archName = aObj->GetArchetypeName().c_str();

    this->GetBrowser()->GetArchetypeTools()->GetLineEdit()->setText(archName);
  }

  ComponentWidget *ComponentTree::CreateComponent(YTE::Type *aType,
    const char *aName,
    YTE::Component *aEngineComp)
  {
    ComponentWidget *comp = new ComponentWidget(aType,
      aName,
      aEngineComp,
      mComponentBrowser->GetMainWindow());
    comp->setObjectName(aName);
    return comp;
  }

  void ComponentTree::SetItemToCollapsedColor(QTreeWidgetItem *aItem)
  {
    aItem->setBackgroundColor(0, "#191919");
  }

  void ComponentTree::SetItemToExpandedColor(QTreeWidgetItem *aItem)
  {
    aItem->setBackgroundColor(0, "#383838");
  }

  void ComponentTree::AddComponent(ComponentWidget *aWidget)
  {
    auto cmd = std::make_unique<AddComponentCmd>(aWidget->GetName().c_str(),
      mOutputConsole);

    mUndoRedo->InsertCommand(std::move(cmd));
    BaseAddComponent(aWidget);
  }

  void ComponentTree::RemoveComponent(QTreeWidgetItem *aWidget)
  {
    auto name = aWidget->text(0).toStdString();
    auto cmd = std::make_unique<RemoveComponentCmd>(name.c_str(),
      mOutputConsole);
    mUndoRedo->InsertCommand(std::move(cmd));
    BaseRemoveComponent(aWidget);
  }

  void ComponentTree::BaseAddComponent(ComponentWidget *aWidget)
  {
    QTreeWidgetItem * item = new QTreeWidgetItem(this);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    item->setSizeHint(0, QSize(0, 27));
    item->setText(0, aWidget->GetName().c_str());

    QTreeWidgetItem * child = new QTreeWidgetItem(item);
    child->setFlags(Qt::NoItemFlags);
    this->setItemWidget(child, 0, aWidget);

    item->addChild(child);

    YTE::String tip = aWidget->GetEngineComponent()->GetType()->Description();

    item->setToolTip(0, tip.c_str());
    child->setToolTip(0, tip.c_str());

    connect(this,
      &QTreeWidget::itemCollapsed,
      this,
      &ComponentTree::SetItemToCollapsedColor);
    connect(this,
      &QTreeWidget::itemExpanded,
      this,
      &ComponentTree::SetItemToExpandedColor);

    this->insertTopLevelItem(this->topLevelItemCount(), item);

    mComponentWidgets.push_back(aWidget);
  }

  void ComponentTree::BaseRemoveComponent(QTreeWidgetItem * aWidget)
  {
    QWidget * itemWidg = this->itemWidget(aWidget->child(0), 0);
    ComponentWidget * compWidg = dynamic_cast<ComponentWidget*>(itemWidg);

    if (!compWidg)
    {
      return;
    }

    if (compWidg->GetName() == "Model")
    {
      mComponentBrowser->GetMainWindow()->GetMaterialViewer().LoadNoMaterial();
    }

    compWidg->RemoveComponentFromEngine();

    mComponentWidgets.erase(std::remove(mComponentWidgets.begin(),
      mComponentWidgets.end(),
      compWidg),
      mComponentWidgets.end());

    aWidget->setHidden(true);
    this->removeItemWidget(aWidget, 0);

    setCurrentItem(topLevelItem(0));
  }

  void ComponentTree::ClearComponents()
  {
    this->clear();
    mComponentWidgets.clear();
  }

  std::vector<ComponentWidget*>& ComponentTree::GetComponentWidgets()
  {
    return mComponentWidgets;
  }

  void ComponentTree::CreateContextMenu(const QPoint & pos)
  {
    QTreeWidgetItem *item = this->itemAt(pos);

    if (item == nullptr)
    {
      return;
    }

    QMenu * contextMenu = new QMenu(this);

    QAction * removeAct = new QAction("Remove", contextMenu);
    connect(removeAct,
      &QAction::triggered,
      this,
      &ComponentTree::RemoveCurrentComponent);

    contextMenu->addAction(removeAct);
    contextMenu->exec(this->mapToGlobal(pos));
  }

  void ComponentTree::RemoveCurrentComponent()
  {
    if (this->topLevelItemCount() == 0)
    {
      return;
    }

    QTreeWidgetItem * currItem = this->currentItem();

    if (!currItem)
    {
      return;
    }

    QTreeWidgetItem * childItem = currItem->child(0);

    if (!childItem)
    {
      return;
    }

    QWidget * itemWidg = this->itemWidget(childItem, 0);

    ComponentWidget * compWidg = dynamic_cast<ComponentWidget*>(itemWidg);

    if (!compWidg)
    {
      return;
    }

    if (compWidg->GetName() == "Model")
    {
      mComponentBrowser->GetMainWindow()->GetMaterialViewer().LoadNoMaterial();
    }

    compWidg->RemoveComponentFromEngine();

    mComponentWidgets.erase(std::remove(mComponentWidgets.begin(),
      mComponentWidgets.end(),
      compWidg),
      mComponentWidgets.end());

    currItem->setHidden(true);
    this->removeItemWidget(currItem, 0);

    setCurrentItem(topLevelItem(0));
  }

  ComponentBrowser * ComponentTree::GetBrowser()
  {
    return mComponentBrowser;
  }

  void ComponentTree::SetWindowSettings()
  {
    this->setHeaderLabel("");
    this->setRootIsDecorated(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->verticalScrollBar()->setSingleStep(1);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setMouseTracking(true);
  }

  void ComponentTree::keyPressEvent(QKeyEvent *aEvent)
  {
    if (aEvent->key() == Qt::Key_Delete)
    {
      RemoveCurrentComponent();
    }
    else
    {
      QTreeWidget::keyPressEvent(aEvent);
    }
  }

}
