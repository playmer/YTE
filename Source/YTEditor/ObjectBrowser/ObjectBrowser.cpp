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

#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmenu.h>
#include <qevent.h>
#include <qheaderview.h>
#include <qmimedata.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"
#include "YTEditor/UndoRedo/UndoRedo.hpp"


namespace YTEditor
{

  ObjectBrowser::ObjectBrowser(MainWindow * aMainWindow, QWidget * parent)
    : QTreeWidget(parent)
    , mMainWindow(aMainWindow)
  {
    SetWidgetSettings();

    // Calls OnCurrentItemChanged() when the currentItemChanged event is received
    connect(this, &QTreeWidget::currentItemChanged,
      this, &ObjectBrowser::OnCurrentItemChanged);

    // item selection for undo redo
    connect(this, &QTreeWidget::itemSelectionChanged,
      this, &ObjectBrowser::OnItemSelectionChanged);

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
    if (std::string(aArchetypeName) == "Gizmo")
    {
      return nullptr;
    }

    auto space = mMainWindow->GetEditingLevel();

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
    auto& spaces = mMainWindow->GetRunningEngine()->GetCompositions();
    auto space = spaces.begin()->second.get();

    auto composition = space->AddComposition(aArchetypeName, aCompositionName);


    auto cmd = std::make_unique<AddObjectCmd>(composition,
      &mMainWindow->GetOutputConsole(),
      this);

    mMainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));
    return AddTreeItem(aCompositionName, aParentObj, composition, aIndex);
  }

  ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName,
                                         YTE::Composition *aEngineObj,
                                         int aIndex,
                                         bool aSetAsCurrent)
  {
    YTEProfileFunction();

    YTE::String name{ aItemName };

    if (std::string(aItemName) == "Gizmo")
    {
      return nullptr;
    }

    YTE::Composition *space = mMainWindow->GetEditingLevel();

    ObjectItem *item = new ObjectItem(name, this, aEngineObj, space);

    // Add new item as a top level member in the tree hierarchy
    // (object should have no parent objects)
    this->insertTopLevelItem(aIndex, item);

    if (aSetAsCurrent)
    {
      this->setCurrentItem(item);
    }

    for (auto const& [name, child] : aEngineObj->GetCompositions())
    {
      AddTreeItem(child->GetName().c_str(), item, child.get(), aSetAsCurrent);
    }

    return item;
  }

  ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName,
                                         ObjectItem *aParentObj,
                                         YTE::Composition *aEngineObj,
                                         int aIndex,
                                         bool aSetAsCurrent)
  {
    YTEProfileFunction();

    YTE::String name{ aItemName };

    auto space = mMainWindow->GetEditingLevel();

    ObjectItem *item = new ObjectItem(name, aParentObj, aEngineObj, space);

    // add this object as a child of another tree item
    aParentObj->insertChild(aIndex, item);

    if (aSetAsCurrent)
    {
      this->setCurrentItem(item);
    }
    
    for (auto const&[name, child] : aEngineObj->GetCompositions())
    {
      AddTreeItem(child->GetName().c_str(), item, child.get(), aSetAsCurrent);
    }

    return item;
  }

  void ObjectBrowser::SetWidgetSettings()
  {
    this->setObjectName("ObjectBrowser");
    this->setMinimumWidth(200);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setMouseTracking(true);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }

  void ObjectBrowser::OnCurrentItemChanged(QTreeWidgetItem *aCurrent,
                                           QTreeWidgetItem *aPrevious)
  {
    ObjectItem *prevObj = aPrevious ? static_cast<ObjectItem*>(aPrevious) : nullptr;
    ObjectItem *currObj = aCurrent ? static_cast<ObjectItem*>(aCurrent) : nullptr;

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

      YTE::Model* model = currObj->GetEngineObject()->GetComponent<YTE::Model>();

      auto matViewer = mMainWindow->GetMaterialViewer();

      if (matViewer && model && model->GetMesh())
      {
        // get the list of materials from the submeshes
        auto& submeshes = model->GetMesh()->mParts;

        matViewer->LoadMaterial(submeshes[0].mUBOMaterial);
        matViewer->SetMaterialsList(&submeshes);
      }
      else
      {
        if (matViewer)
        {
          matViewer->LoadNoMaterial();
        }
      }

      // get the transform of the currently selected object
      YTE::Transform *currTransform = currObj->GetEngineObject()->GetComponent<YTE::Transform>();

      if (currTransform)
      {
        Gizmo *giz = mMainWindow->GetGizmo();

        if (giz)
        {
          giz->SnapToCurrentObject();
        }
      }
    }
  }

  void ObjectBrowser::OnItemSelectionChanged()
  {
    QList<QTreeWidgetItem*> items = this->selectedItems();

    OutputConsole *console = &mMainWindow->GetOutputConsole();

    std::vector<YTE::GlobalUniqueIdentifier> newSelection;
    std::vector<YTE::GlobalUniqueIdentifier> oldSelection;

    for (auto item : items)
    {
      ObjectItem *objItem = static_cast<ObjectItem*>(item);

      auto guid = objItem->GetEngineObject()->GetGUID();

      newSelection.push_back(guid);
    }

    for (auto item : mSelectedItems)
    {
      oldSelection.push_back(item);
    }

    if (mInsertSelectionChangedCmd)
    {
      UndoRedo *undoRedo = mMainWindow->GetUndoRedo();
      undoRedo->InsertCommand(std::make_unique<ObjectSelectionChangedCmd>(newSelection, oldSelection, this, console));
    }

    mSelectedItems = newSelection;

    /*
    // if objects were selected
    if (items.size() > mSelectedItems.size())
    {
      QList<QTreeWidgetItem*> delta = items;

      for (auto item : mSelectedItems)
      {
        delta.removeOne(item);
      }

      std::vector<YTE::GlobalUniqueIdentifier> guids;

      for (auto item : delta)
      {
        ObjectItem *objItem = static_cast<ObjectItem*>(item);

        auto guid = objItem->GetEngineObject()->GetGUID();

        guids.push_back(guid);
      }

      UndoRedo *undoRedo = mMainWindow->GetUndoRedo();
      undoRedo->InsertCommand(std::make_unique<ObjectsSelectedCmd>(guids, this, console));
    }
    // if objects were unselected
    else if (items.size() < mSelectedItems.size())
    {
      QList<QTreeWidgetItem*> delta = mSelectedItems;

      for (auto item : items)
      {
        delta.removeOne(item);
      }

      std::vector<YTE::GlobalUniqueIdentifier> guids;

      for (auto item : delta)
      {
        ObjectItem *objItem = static_cast<ObjectItem*>(item);

        auto guid = objItem->GetEngineObject()->GetGUID();

        guids.push_back(guid);
      }
    }
    */
  }


  void ObjectBrowser::DuplicateCurrentlySelected()
  {
    YTE::Composition *currentObj = GetCurrentObject();
    if (currentObj == nullptr)
    {
      return;
    }
    
    YTE::RSAllocator allocator;
    YTE::RSValue serialized = currentObj->Serialize(allocator);

    YTE::RSStringBuffer sb;
    YTE::RSPrettyWriter writer(sb);
    serialized.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    std::string levelInJson = sb.GetString();

    YTE::Composition *duplicate = mMainWindow->GetEditingLevel()->AddComposition(&serialized, "Copy");

    YTE::String guid = duplicate->GetGUID().ToString();
    duplicate->SetName(guid);

    AddExistingComposition(guid.c_str(), duplicate);
  }

  void ObjectBrowser::OnItemTextChanged(QTreeWidgetItem *aItem, int aIndex)
  {
    (void)aIndex;

    ObjectItem *currItem = static_cast<ObjectItem*>(aItem);

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

  void ObjectBrowser::dropEvent(QDropEvent *aEvent)
  {
    YTE::Composition *movedObj = GetCurrentObject();

    ObjectItem *parentItem = static_cast<ObjectItem*>(itemAt(aEvent->pos()));

    YTE::Composition *parentObj = nullptr;

    if (parentItem)
    {
      parentObj = parentItem->GetEngineObject();
    }

    movedObj->ReParent(parentObj);
    
    QTreeWidget::dropEvent(aEvent);
  }

  void ObjectBrowser::CreateContextMenu(const QPoint &pos)
  {
    QTreeWidgetItem *item = this->itemAt(pos);

    if (item == nullptr)
    {
      return;
    }

    QMenu *contextMenu = new QMenu(this);

    QAction *removeAct = new QAction("Remove", contextMenu);
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

    YTE::Composition *engineObj = currItem->GetEngineObject();

    auto name = currItem->text(0).toStdString();
    auto cmd = std::make_unique<RemoveObjectCmd>(engineObj, &mMainWindow->GetOutputConsole(), &mMainWindow->GetObjectBrowser());

    mMainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));

    // remove current object from engine
    currItem->DeleteFromEngine();

    RemoveObjectFromViewer(currItem);
  }

  void ObjectBrowser::SetInsertSelectionChangedCommand(bool isActive)
  {
    mInsertSelectionChangedCmd = isActive;
  }

  void ObjectBrowser::MoveToFrontOfCamera(YTE::Composition *aObject)
  {
    if (YTE::Transform *transform = aObject->GetComponent<YTE::Transform>())
    {
      YTE::Composition *camera = mMainWindow->GetEditorCamera();

      YTE::Transform *camTransform = camera->GetComponent<YTE::Transform>();
      YTE::Orientation *orientation = camera->GetComponent<YTE::Orientation>();

      if (camTransform && orientation)
      {
        glm::vec3 newPos = camTransform->GetWorldTranslation() - 10.0f * orientation->GetForwardVector();

        transform->SetWorldTranslation(newPos);
      }
    }
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

    auto matViewer = mMainWindow->GetMaterialViewer();

    if (matViewer && currItem && currItem->GetEngineObject())
    {
      YTE::Model *model = currItem->GetEngineObject()->GetComponent<YTE::Model>();

      if (model)
      {
        YTE::Mesh *mesh = model->GetMesh();
        
        if (mesh)
        {
          matViewer->LoadMaterial(mesh->mParts[0].mUBOMaterial);

          // get the list of materials from the submeshes
          auto& submeshes = mesh->mParts;

          matViewer->SetMaterialsList(&submeshes);
        }
      }
    }

    if (topLevelItemCount() == 0)
    {
      mMainWindow->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Select);
    }
  }

  void ObjectBrowser::keyPressEvent(QKeyEvent *aEvent)
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

  void ObjectBrowser::LoadAllChildObjects(YTE::Composition *aParentObj, ObjectItem *aParentItem)
  {
    return;

    // if the parent object has no children
    if (aParentObj->GetCompositions().size() == 0)
    {
      return;
    }

    for (auto& cmp : aParentObj->GetCompositions())
    {
      ObjectItem * item = AddTreeItem(cmp.first.c_str(), aParentItem, cmp.second.get(), 0, false);

      if (item)
      {
        LoadAllChildObjects(cmp.second.get(), item);
      }
    }
  }

  YTE::Composition* ObjectBrowser::GetCurrentObject()
  {
    auto objItem = dynamic_cast<ObjectItem*>(currentItem());

    if (objItem)
    {
      return objItem->GetEngineObject();
    }
    else
    {
      return nullptr;
    }

  }

  ObjectItem* ObjectBrowser::SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp)
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

  void ObjectBrowser::FindObjectsByArchetypeInternal(YTE::String &aArchetypeName,
                                                     std::vector<ObjectItem*>& aResult,
                                                     ObjectItem* aItem)
  {
    for (int i = 0; i < aItem->childCount(); ++i)
    {
      ObjectItem *item = dynamic_cast<ObjectItem*>(aItem->child(i));

      if (item->GetEngineObject()->GetArchetypeName() == aArchetypeName)
      {
        aResult.emplace_back(item);
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

  MainWindow * ObjectBrowser::GetMainWindow() const
  {
    return mMainWindow;
  }

  std::vector<ObjectItem*> ObjectBrowser::FindAllObjectsOfArchetype(YTE::String &aArchetypeName)
  {
    std::vector<ObjectItem*> result;

    // loop through all items in the object browser
    for (int i = 0; i < topLevelItemCount(); ++i)
    {
      ObjectItem *objItem = dynamic_cast<ObjectItem*>(topLevelItem(i));

      if (objItem->GetEngineObject()->GetArchetypeName() == aArchetypeName)
      {
        result.emplace_back(objItem);
      }

      FindObjectsByArchetypeInternal(aArchetypeName, result, objItem);
    }

    return std::move(result);
  }

  void ObjectBrowser::SelectNoItem()
  {
    this->setCurrentItem(nullptr);
  }

}
