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
#include <qmimedata.h>

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Meta/Meta.hpp"
#include "YTE/Utilities/JsonHelpers.hpp"

#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/ComponentBrowser/HeaderListWidget.hpp"
#include "YTEditor/ComponentBrowser/PropertyWidget.hpp"

#include "YTEditor/FileViewer/FileViewer.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"

#include "YTEditor/MaterialViewer/MaterialViewer.hpp"

#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"

#include "YTEditor/UndoRedo/UndoRedo.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{

  ComponentTree::ComponentTree(ComponentBrowser *parent)
    : QTreeWidget(parent)
    , mComponentBrowser(parent)
    , mUndoRedo(mComponentBrowser->GetMainWindow()->GetUndoRedo())
    , mOutputConsole(&(mComponentBrowser->GetMainWindow()->GetOutputConsole()))
  {
    SetWindowSettings();

    connect(this,
      &QTreeWidget::customContextMenuRequested,
      this,
      &ComponentTree::CreateContextMenu);

    setItemDelegate(new ComponentDelegate(this));

    setDropIndicatorShown(true);

    viewport()->setAcceptDrops(true);

    this->setAcceptDrops(true);

    setDragDropOverwriteMode(true);
  }

  ComponentTree::~ComponentTree()
  {
  }

  void ComponentTree::LoadGameObject(YTE::Composition *aObj)
  {
    this->ClearComponents();

    // Load all of the components on the object into the browser
    YTE::ComponentMap &components = *(aObj->GetComponents());

    for (auto& comp : components)
    {
      YTE::String name = comp.first->GetName();

      QTreeWidgetItem *item = new QTreeWidgetItem(this);
      item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
      item->setSizeHint(0, QSize(0, 27));

      ComponentWidget *widg = CreateComponent(comp.second.get()->GetType(),
                                              name.c_str(),
                                              comp.second.get(),
                                              item);

      widg->LoadProperties(comp.second.get());
      BaseAddComponent(widg, item);
    }

    QString archName = aObj->GetArchetypeName().c_str();

    this->GetBrowser()->GetArchetypeTools()->GetLineEdit()->setText(archName);
  }

  ComponentWidget *ComponentTree::CreateComponent(YTE::Type *aType,
                                                  const char *aName,
                                                  YTE::Component *aEngineComp,
                                                  QTreeWidgetItem *aTopItem)
  {
    ComponentWidget *comp = new ComponentWidget(aType,
                                                aName,
                                                aEngineComp,
                                                mComponentBrowser->GetMainWindow(),
                                                aTopItem);

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

  QTreeWidgetItem* ComponentTree::FindComponentItem(YTE::Type *aComponentType)
  {
    for (auto &widget : mComponentWidgets)
    {
      YTE::Type *type = widget->GetEngineComponent()->GetType();

      if (aComponentType == type)
      {
        return widget->GetParentItem();
      }
    }

    return nullptr;
  }

  ComponentWidget* ComponentTree::AddComponent(YTE::Type *aComponentType)
  {
    ComponentWidget *widg = InternalAddComponent(aComponentType);
    auto cmd = std::make_unique<AddComponentCmd>(widg->GetEngineComponent(), 
                                                 mComponentBrowser, 
                                                 mOutputConsole);
    mUndoRedo->InsertCommand(std::move(cmd));

    return widg;
  }

  void ComponentTree::RemoveComponent(QTreeWidgetItem *aWidget)
  {
    ComponentWidget *compWidg = dynamic_cast<ComponentWidget*>(aWidget);

    if (!compWidg)
    {
      mOutputConsole->PrintLnC(OutputConsole::Color::Red, 
        "Trying to remove a tree item from ComponentTree that isn't a component widget.");
      
      return;
    }

    auto component = compWidg->GetEngineComponent();
    auto owner = component->GetOwner();
    auto reason = owner->IsDependecy(component->GetType());

    if (reason.size())
    {
      owner->GetEngine()->Log(YTE::LogType::Error, reason);
      return;
    }

    auto name = aWidget->text(0).toStdString();
    auto cmd = std::make_unique<RemoveComponentCmd>(compWidg->GetEngineComponent(), 
                                                    mComponentBrowser, 
                                                    mOutputConsole);
    mUndoRedo->InsertCommand(std::move(cmd));
    BaseRemoveComponent(aWidget);
  }

  ComponentWidget* ComponentTree::InternalAddComponent(YTE::Type *aComponentType)
  {
    MainWindow *mainWindow = mComponentBrowser->GetMainWindow();
    ObjectBrowser &objBrowser = mainWindow->GetObjectBrowser();

    YTE::Composition *currentObj = objBrowser.GetCurrentObject();

    YTE::Component *component = currentObj->AddComponent(aComponentType, true);

    YTE::String name = component->GetType()->GetName();

    QTreeWidgetItem *item = new QTreeWidgetItem(this);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    item->setSizeHint(0, QSize(0, 27));

    ComponentWidget *widg = CreateComponent(component->GetType(),
      name.c_str(),
      component,
      item);

    widg->LoadProperties(component);
    BaseAddComponent(widg, item);

    return widg;
  }

  ComponentWidget* ComponentTree::InternalAddComponent(YTE::Type *aComponentType, YTE::RSValue *aSerializedComponent)
  {
    MainWindow *mainWindow = mComponentBrowser->GetMainWindow();
    ObjectBrowser &objBrowser = mainWindow->GetObjectBrowser();

    YTE::Composition *currentObj = objBrowser.GetCurrentObject();

    YTE::Component *component = currentObj->AddComponent(aComponentType, aSerializedComponent);

    YTE::String name = component->GetType()->GetName();

    QTreeWidgetItem *item = new QTreeWidgetItem(this);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    item->setSizeHint(0, QSize(0, 27));

    ComponentWidget *widg = CreateComponent(component->GetType(),
                                            name.c_str(),
                                            component,
                                            item);

    widg->LoadProperties(component);
    BaseAddComponent(widg, item);

    return widg;
  }

  void ComponentTree::BaseAddComponent(ComponentWidget *aWidget, QTreeWidgetItem *aTopItem)
  {
    aTopItem->setText(0, aWidget->GetName().c_str());

    QTreeWidgetItem *child = new QTreeWidgetItem(aTopItem);
    child->setFlags(Qt::NoItemFlags);
    this->setItemWidget(child, 0, aWidget);

    aTopItem->addChild(child);

    YTE::Type *componentType = aWidget->GetEngineComponent()->GetType();

    YTE::String tip = componentType->Description();

    aTopItem->setToolTip(0, tip.c_str());
    child->setToolTip(0, tip.c_str());

    connect(this,
            &QTreeWidget::itemCollapsed,
            this,
            &ComponentTree::SetItemToCollapsedColor);
    
    connect(this,
            &QTreeWidget::itemExpanded,
            this,
            &ComponentTree::SetItemToExpandedColor);

    insertTopLevelItem(this->topLevelItemCount(), aTopItem);

    mComponentWidgets.push_back(aWidget);
  }

  void ComponentTree::BaseRemoveComponent(QTreeWidgetItem *aWidget)
  {
    // loop through all the children
    for (int i = 0; i < aWidget->childCount(); ++i)
    {
      QWidget *itemWidg = this->itemWidget(aWidget->child(i), 0);

      if (itemWidg)
      {
        ComponentWidget *compWidg = dynamic_cast<ComponentWidget*>(itemWidg);

        if (!compWidg)
        {
          continue;
        }

        if (compWidg->GetName() == "Model")
        {
          mComponentBrowser->GetMainWindow()->GetMaterialViewer()->LoadNoMaterial();
        }

        compWidg->RemoveComponentFromEngine();

        mComponentWidgets.erase(std::remove(mComponentWidgets.begin(),
                                            mComponentWidgets.end(),
                                            compWidg),
                                mComponentWidgets.end());
      }
      else
      {
        QTreeWidgetItem *childItem = aWidget->child(i);

        QWidget *headerWidgBase = this->itemWidget(childItem->child(0), 0);

        if (headerWidgBase)
        {
          HeaderListWidget *headerWidg = dynamic_cast<HeaderListWidget*>(headerWidgBase);

          if (!headerWidg)
          {
            continue;
          }

          headerWidg->RemoveSelf();
        }
      }
    }
    
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

    QMenu *contextMenu = new QMenu(this);

    QAction *removeAct = new QAction("Remove", contextMenu);
    
    connect(removeAct,
            &QAction::triggered,
            this,
            &ComponentTree::RemoveCurrentTreeItem);

    contextMenu->addAction(removeAct);
    contextMenu->exec(this->mapToGlobal(pos));
  }

  void ComponentTree::RemoveCurrentTreeItem()
  {
    QTreeWidgetItem *item = currentItem();

    QString currName = item->text(0);

    std::cout << "Current Item: " << currName.toStdString() << std::endl;

    QWidget *itemWidg = this->itemWidget(item->child(item->childCount() - 1), 0);

    ComponentWidget *componentWidget = dynamic_cast<ComponentWidget*>(itemWidg);

    if (componentWidget)
    {
      RemoveCurrentComponent();
      return;
    }

    HeaderListWidget *headerWidg = dynamic_cast<HeaderListWidget*>(itemWidg);

    if (headerWidg)
    {
      RemoveCurrentHeaderListWidget();
      return;
    }
  }

  void ComponentTree::RemoveCurrentComponent()
  {
    if (this->topLevelItemCount() == 0)
    {
      return;
    }

    QTreeWidgetItem *currItem = this->currentItem();

    if (!currItem)
    {
      return;
    }

    QTreeWidgetItem *childItem = currItem->child(currItem->childCount() - 1);

    if (!childItem)
    {
      return;
    }

    QWidget *itemWidg = this->itemWidget(childItem, 0);

    ComponentWidget *compWidg = dynamic_cast<ComponentWidget*>(itemWidg);

    if (!compWidg)
    {
      return;
    }

    //HeaderListWidget *headerWidg = dynamic_cast<HeaderListWidget*>(itemWidg);
    //
    //if (headerWidg)
    //{
    //  this->itemWidget(currItem->parent(), 0);
    //}

    if (compWidg->GetName() == "Model")
    {
      auto mainWin = mComponentBrowser->GetMainWindow();
      auto matViewer = mainWin->GetMaterialViewer();

      if (matViewer)
      {
        matViewer->LoadNoMaterial();
      }
    }

    auto component = compWidg->GetEngineComponent();
    YTE::Composition *owner = compWidg->GetEngineComponent()->GetOwner();

    auto reason = owner->IsDependecy(component->GetType());

    if (reason.size())
    {
      owner->GetEngine()->Log(YTE::LogType::Error, reason);
      return;
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

  void ComponentTree::RemoveCurrentHeaderListWidget()
  {
    QTreeWidgetItem *item = currentItem();

    QWidget *itemWidg = this->itemWidget(item->child(0), 0);

    HeaderListWidget *headerWidg = dynamic_cast<HeaderListWidget*>(itemWidg);

    if (headerWidg)
    {
      YTE::Component *component = headerWidg->GetEngineComponent();

      YTE::Type *type = component->GetType();

      if (type->IsA<YTE::Animator>())
      {
        YTE::Animator *animator = static_cast<YTE::Animator*>(component);

        YTE::Object *object = headerWidg->GetEngineObject();

        animator->RemoveAnimation(static_cast<YTE::Animation*>(object));

        item->setHidden(true);
        this->removeItemWidget(item->child(0), 0);
      }
    }
  }

  ComponentBrowser* ComponentTree::GetBrowser()
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
      RemoveCurrentTreeItem();
    }
    else
    {
      QTreeWidget::keyPressEvent(aEvent);
    }
  }

  void ComponentTree::dropEvent(QDropEvent *aEvent)
  {
    QObject *sourceObj = aEvent->source();

    ObjectBrowser *objBrowser = dynamic_cast<ObjectBrowser*>(sourceObj);

    if (objBrowser)
    {
      aEvent->ignore();
    }

    FileViewer *fileViewer = dynamic_cast<FileViewer*>(sourceObj);

    if (fileViewer)
    {
      const QMimeData *rawData = aEvent->mimeData();

      QByteArray encodedData = rawData->data("application/vnd.text.list");
      QDataStream stream(&encodedData, QIODevice::ReadOnly);
      QStringList newItems;

      int rows = 0;

      while (!stream.atEnd())
      {
        QString text;
        stream >> text;
        newItems << text;
        ++rows;
      }

      std::string filename = newItems[0].toStdString();

      QTreeWidgetItem *item = this->itemAt(aEvent->pos());
      QWidget *widget = this->itemWidget(item->child(item->childCount() - 1), 0);

      ComponentWidget *componentWidget = static_cast<ComponentWidget*>(widget);
      
      YTE::Component *component = componentWidget->GetEngineComponent();
      YTE::Animator *animator = static_cast<YTE::Animator*>(component);
      YTE::Animation *animation = animator->AddAnimation(filename);

      if (animation == nullptr)
      {
        aEvent->ignore();
        return;
      }

      LoadGameObject(animator->GetOwner());
    }

    aEvent->ignore();
  }

  void ComponentTree::dragEnterEvent(QDragEnterEvent *aEvent)
  {
    aEvent->acceptProposedAction();
  }

  void ComponentTree::dragMoveEvent(QDragMoveEvent *aEvent)
  {
    QTreeWidgetItem *item = this->itemAt(aEvent->pos());
    
    // check if the cursor is hovering over an item
    if (item)
    {
      QWidget *widget = this->itemWidget(item, 0);

      // check if the cursor is hovering over the body of the component
      if (widget)
      {
        //HeaderListWidget *headerWidg = dynamic_cast<HeaderListWidget*>(widget);
        //
        //// check if the body is a header list widget
        //if (headerWidg)
        //{
        //  YTE::Component *component = headerWidg->GetEngineComponent();
        //
        //  YTE::Animator *animator = dynamic_cast<YTE::Animator*>(component);
        //
        //  // check if it's an animator component
        //  if (animator)
        //  {
        //    aEvent->acceptProposedAction();
        //    return;
        //  }
        //}
        //else
        //{
        //  ComponentWidget *componentWidget = dynamic_cast<ComponentWidget*>(widget);
        //
        //  if (componentWidget)
        //  {
        //    YTE::Component *component = componentWidget->GetEngineComponent();
        //
        //    YTE::Animator *animator = dynamic_cast<YTE::Animator*>(component);
        //
        //    // check if it's an animator component
        //    if (animator)
        //    {
        //      aEvent->acceptProposedAction();
        //      return;
        //    }
        //  }
        //}
      }
      else
      {
        QString itemText = item->text(0);

        if (item->text(0) == "Animator")
        {
          aEvent->acceptProposedAction();
          return;
        }
      }
    }

    aEvent->setAccepted(false);
  }
}
