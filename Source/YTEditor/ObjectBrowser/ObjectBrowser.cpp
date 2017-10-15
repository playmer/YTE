/******************************************************************************/
/*!
\file   ObjectBrowser.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the Object Browser.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <stack>

#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qpushbutton.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <QtCore/qtextstream.h>
#include <qmenu.h>
#include <qevent.h>
#include <qheaderview.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Utilities/String/String.h"

#include "YTE/Graphics/InstantiatedMesh.hpp"
#include "YTE/Graphics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"

#include "../ComponentBrowser/ArchetypeTools.hpp"
#include "../ComponentBrowser/ComponentBrowser.hpp"
#include "../ComponentBrowser/ComponentTree.hpp"
#include "../MainWindow/YTEditorMainWindow.hpp"
#include "../MaterialViewer/MaterialViewer.hpp"
#include "../UndoRedo/Commands.hpp"
#include "../UndoRedo/UndoRedo.hpp"

#include "ObjectBrowser.hpp"
#include "ObjectItem.hpp"


ObjectBrowser::ObjectBrowser(YTEditorMainWindow * aMainWindow, QWidget * parent)
  : QTreeWidget(parent), 
    mMainWindow(aMainWindow)
{
  SetWidgetSettings();

  // Calls OnCurrentItemChanged() when the currentItemChanged event is received
  connect(this, &QTreeWidget::currentItemChanged, 
          this, &ObjectBrowser::OnCurrentItemChanged);

  connect(this, &QTreeWidget::customContextMenuRequested, 
          this, &ObjectBrowser::CreateContextMenu);

  connect(this, &QTreeWidget::itemChanged, 
          this, &ObjectBrowser::OnItemTextChanged);

  this->setItemDelegate(new ObjectItemDelegate(this));
}

ObjectBrowser::~ObjectBrowser()
{
}

void ObjectBrowser::ClearObjectBrowser()
{
  this->clear();
}

ObjectItem* ObjectBrowser::AddObject(const char *aCompositionName,
                                     const char *aArchetypeName,
                                     int aIndex)
{
  auto spaces = mMainWindow->GetRunningEngine()->GetCompositions();
  auto space = spaces->begin()->second.get();

  auto composition = space->AddComposition(aArchetypeName, aCompositionName);

  auto cmd = std::make_unique<AddObjectCmd>(composition,
                                            &mMainWindow->GetOutputConsole(),
                                            this);

  mMainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));
  return AddTreeItem(aCompositionName, composition, aIndex);
}

ObjectItem* ObjectBrowser::AddExistingComposition(const char *aCompositionName,
                                                  YTE::Composition *aComposition)
{
  return AddTreeItem(aCompositionName, aComposition);
}

ObjectItem* ObjectBrowser::AddChildObject(const char *aCompositionName,
                                          const char *aArchetypeName,
                                          ObjectItem *aParentObj,
                                          int aIndex)
{
  auto spaces = mMainWindow->GetRunningEngine()->GetCompositions();
  auto space = spaces->begin()->second.get();

  auto composition = space->AddComposition(aArchetypeName, aCompositionName);

  
  auto cmd = std::make_unique<AddObjectCmd>(composition,
                                            &mMainWindow->GetOutputConsole(),
                                            this);

  mMainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));
  return AddTreeItem(aCompositionName, aParentObj, composition, aIndex);
}

ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName, 
                                       YTE::Composition *aEngineObj,
                                       int aIndex)
{
  YTE::String name{ aItemName };

  auto spaces = mMainWindow->GetRunningEngine()->GetCompositions();
  YTE::Composition *space = spaces->begin()->second.get();

  ObjectItem *item = new ObjectItem(name, this, aEngineObj, space);

  // Add new item as a top level member in the tree heirarchy
  // (object should have no parent objects)
  this->insertTopLevelItem(aIndex, item);

  this->setCurrentItem(item);

  return item;
}

ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName, 
                                       ObjectItem *aParentObj, 
                                       YTE::Composition *aEngineObj,
                                       int aIndex)
{
  YTE::String name{ aItemName };

  auto spaces = mMainWindow->GetRunningEngine()->GetCompositions();
  YTE::Composition *space = spaces->begin()->second.get();

  ObjectItem *item = new ObjectItem(name, aParentObj, aEngineObj, space);

  // add this object as a child of another tree item
  aParentObj->insertChild(aIndex, item);

  this->setCurrentItem(item);

  return item;
}

void ObjectBrowser::SetWidgetSettings()
{
  this->setObjectName("ObjectBrowser");
  this->setMinimumWidth(200);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setDragDropMode(QAbstractItemView::InternalMove);
  this->setMouseTracking(true);
}

void ObjectBrowser::OnCurrentItemChanged(QTreeWidgetItem *aCurrent,
    QTreeWidgetItem *aPrevious)
{
  (void)aPrevious;

  ObjectItem *currObj = dynamic_cast<ObjectItem*>(aCurrent);

  ArchetypeTools *archTools = GetMainWindow()->GetComponentBrowser().GetArchetypeTools();


  if (currObj && currObj->GetEngineObject())
  {
    if (currObj->GetEngineObject()->GetArchetypeName().Empty())
    {
      archTools->SetButtonMode(ArchetypeTools::Mode::NoArchetype);
    }
    else if (currObj->GetEngineObject()->SameAsArchetype())
    {
      archTools->SetButtonMode(ArchetypeTools::Mode::IsSame);
    }
    else
    {
      archTools->SetButtonMode(ArchetypeTools::Mode::HasChanged);
    }

    // Load the new current object into the component browser
    mMainWindow->GetComponentBrowser().GetComponentTree()->ClearComponents();

    mMainWindow->GetComponentBrowser().GetComponentTree()->LoadGameObject(currObj->GetEngineObject());

    if (YTE::Model* model = currObj->GetEngineObject()->GetComponent<YTE::Model>())
    {
      // get the list of materials from the submeshes
      auto& submeshes = model->GetInstantiatedMesh()->mMesh->mParts;

      mMainWindow->GetMaterialViewer().LoadMaterial(submeshes[0].mUBOMaterial);
      mMainWindow->GetMaterialViewer().SetMaterialsList(&submeshes);
    }
    else
    {
      mMainWindow->GetMaterialViewer().LoadNoMaterial();
    }
  }
}

void ObjectBrowser::OnItemTextChanged(QTreeWidgetItem * aItem, int aIndex)
{
  (void)aIndex;

  ObjectItem * currItem = dynamic_cast<ObjectItem*>(aItem);

  if (currItem->GetEngineObject() == nullptr)
  {
    return;
  }

  QString name = aItem->text(0);

  if (name.isEmpty())
  {
    return;
  }

  std::string stdName = name.toStdString();
  YTE::String yteName = stdName.c_str();

  currItem->Rename(yteName);
}

void ObjectBrowser::CreateContextMenu(const QPoint & pos)
{
  QTreeWidgetItem *item = this->itemAt(pos);

  if (item == nullptr)
  {
    return;
  }

  QMenu * contextMenu = new QMenu(this);

  QAction * removeAct = new QAction("Remove", contextMenu);
  connect(removeAct, &QAction::triggered, this, &ObjectBrowser::RemoveCurrentObject);

  contextMenu->addAction(removeAct);
  contextMenu->exec(this->mapToGlobal(pos));
}

void ObjectBrowser::RemoveCurrentObject()
{
  if (topLevelItemCount() == 0)
  {
    return;
  }

  ObjectItem *currItem = dynamic_cast<ObjectItem*>(currentItem());

  if (!currItem || !currItem->GetEngineObject())
  {
    return;
  }

  auto name = currItem->text(0).toStdString();
  auto cmd = std::make_unique<RemoveObjectCmd>(name.c_str(), 
                                               &mMainWindow->GetOutputConsole());

  mMainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));

  // remove current object from engine
  currItem->DeleteFromEngine();

  RemoveObjectFromViewer(currItem);
}

void ObjectBrowser::RemoveObjectFromViewer(ObjectItem *aItem)
{
  // clear the component viewer
  mMainWindow->GetComponentBrowser().GetComponentTree()->ClearComponents();

  // hide and remove from the tree
  aItem->setHidden(true);

  auto parent = aItem->parent();
  
  if (nullptr == parent)
  {
    int index = this->indexOfTopLevelItem(aItem);
    auto item = takeTopLevelItem(index);
    delete item;
  }
  else
  {
    int index = parent->indexOfChild(aItem);
    auto item = parent->takeChild(index);
    delete item;
  }

  setCurrentItem(topLevelItem(0));

  ObjectItem *currItem = dynamic_cast<ObjectItem*>(currentItem());

  if (currItem && currItem->GetEngineObject())
  {
    YTE::Model * model = currItem->GetEngineObject()->GetComponent<YTE::Model>();

    if (model)
    {
      mMainWindow->GetMaterialViewer().LoadMaterial(model->GetInstantiatedMesh()->mMesh->mParts[0].mUBOMaterial);

      // get the list of materials from the submeshes
      auto& submeshes = model->GetInstantiatedMesh()->mMesh->mParts;

      mMainWindow->GetMaterialViewer().SetMaterialsList(&submeshes);
    }
  }
}

void ObjectBrowser::keyPressEvent(QKeyEvent * aEvent)
{
  if (aEvent->key() == Qt::Key_Delete)
  {
    RemoveCurrentObject();
  }
  else
  {
    QTreeWidget::keyPressEvent(aEvent);
  }
}

void ObjectBrowser::LoadAllChildObjects(YTE::Composition * aParentObj, ObjectItem * aParentItem)
{
  // if the parent object has no children
  if (aParentObj->GetCompositions()->size() == 0)
  {
    return;
  }

  for (auto& cmp : *(aParentObj->GetCompositions()))
  {
    ObjectItem * item = AddTreeItem(cmp.first.c_str(), aParentItem, cmp.second.get());
    LoadAllChildObjects(cmp.second.get(), item);
  }
}

YTE::Composition * ObjectBrowser::GetCurrentObject()
{
  return dynamic_cast<ObjectItem*>(currentItem())->GetEngineObject();
}

ObjectItem *ObjectBrowser::SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp)
{
  for (int i = 0; i < aItem->childCount(); ++i)
  {
    ObjectItem *item = dynamic_cast<ObjectItem*>(aItem->child(i));

    if (item->GetEngineObject() == aComp)
    {
      return item;
    }
    else
    {
      ObjectItem *result = SearchChildrenByComp(item, aComp);

      if (result)
      {
        return result;
      }
    }
  }

  return nullptr;
}

void ObjectBrowser::FindObjectsByArchetypeInternal(YTE::String & aArchetypeName, 
                                                   YTE::vector<ObjectItem*>* aResult, 
                                                   ObjectItem* aItem)
{
  for (int i = 0; i < aItem->childCount(); ++i)
  {
    ObjectItem *item = dynamic_cast<ObjectItem*>(aItem->child(i));

    if (item->GetEngineObject()->GetArchetypeName() == aArchetypeName)
    {
      aResult->emplace_back(item);
    }

    FindObjectsByArchetypeInternal(aArchetypeName, aResult, item);
  }
}

ObjectItem* ObjectBrowser::FindItemByComposition(YTE::Composition *aComp)
{
  for (int i = 0; i < this->topLevelItemCount(); ++i)
  {
    ObjectItem *item = dynamic_cast<ObjectItem*>(this->topLevelItem(i));

    if (item->GetEngineObject() == aComp)
    {
      return item;
    }
    else
    {
      ObjectItem *result = SearchChildrenByComp(item, aComp);

      if (result)
      {
        return result;
      }
    }
  }

  return nullptr;
}

YTEditorMainWindow * ObjectBrowser::GetMainWindow() const
{
  return mMainWindow;
}

YTE::vector<ObjectItem*>* ObjectBrowser::FindAllObjectsOfArchetype(YTE::String & aArchetypeName)
{
  YTE::vector<ObjectItem*> *result = new YTE::vector<ObjectItem*>();

  // loop through all items in the object browser
  for (int i = 0; i < topLevelItemCount(); ++i)
  {
    ObjectItem* objItem = dynamic_cast<ObjectItem*>(topLevelItem(i));

    if (objItem->GetEngineObject()->GetArchetypeName() == aArchetypeName)
    {
      result->emplace_back(objItem);
    }

    FindObjectsByArchetypeInternal(aArchetypeName, result, objItem);
  }

  return result;
}

void ObjectBrowser::SelectNoItem()
{
  this->setCurrentItem(nullptr);
}