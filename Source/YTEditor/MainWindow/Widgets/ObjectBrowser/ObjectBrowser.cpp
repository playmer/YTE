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
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Utilities/String/String.hpp"
#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/MainWindow/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/MainWindow/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/MainWindow/Widgets/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/MainWindow/Toolbars/GizmoToolbar.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"
#include "YTEditor/UndoRedo/UndoRedo.hpp"


namespace YTEditor
{

  ObjectBrowser::ObjectBrowser(MainWindow * aMainWindow)
    : Widget(aMainWindow)
    , mTree()
  {
    SetWidgetSettings();

    // Calls OnCurrentItemChanged() when the currentItemChanged event is received
    mTree.connect(&mTree, &QTreeWidget::currentItemChanged, this, &ObjectBrowser::OnCurrentItemChanged);

    // item selection for undo redo
    mTree.connect(&mTree, &QTreeWidget::itemSelectionChanged, this, &ObjectBrowser::OnItemSelectionChanged);

    mTree.connect(&mTree, &QTreeWidget::customContextMenuRequested, this, &ObjectBrowser::CreateContextMenu);

    mTree.connect(&mTree, &QTreeWidget::itemChanged, this, &ObjectBrowser::OnItemTextChanged);

    mTree.setItemDelegate(new ObjectItemDelegate(this));
  }

  void ObjectBrowser::ClearObjectList()
  {
    mTree.clear();
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

    OutputConsole* console = mMainWindow->GetWidget<OutputConsole>("OutputConsole");

    auto cmd = std::make_unique<AddObjectCmd>(composition, console, this);

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

    OutputConsole* console = mMainWindow->GetWidget<OutputConsole>("OutputConsole");

    auto cmd = std::make_unique<AddObjectCmd>(composition, console, this);

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
    mTree.insertTopLevelItem(aIndex, item);

    if (aSetAsCurrent)
    {
      mTree.setCurrentItem(item);
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
      mTree.setCurrentItem(item);
    }
    
    for (auto const&[name, child] : aEngineObj->GetCompositions())
    {
      AddTreeItem(child->GetName().c_str(), item, child.get(), aSetAsCurrent);
    }

    return item;
  }

  void ObjectBrowser::SetWidgetSettings()
  {
    setObjectName("ObjectBrowser");
    setMinimumWidth(200);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);

    mTree.setDragDropMode(QAbstractItemView::InternalMove);
    mTree.setSelectionMode(QAbstractItemView::ExtendedSelection);
  }

  void ObjectBrowser::OnCurrentItemChanged(QTreeWidgetItem *aCurrent,
                                           QTreeWidgetItem *aPrevious)
  {
    ObjectItem *prevItem = aPrevious ? static_cast<ObjectItem*>(aPrevious) : nullptr;
    ObjectItem *currItem = aCurrent ? static_cast<ObjectItem*>(aCurrent) : nullptr;

    ComponentBrowser* componentBrowser = mMainWindow->GetWidget<ComponentBrowser>("ComponentBrowser");

    if (componentBrowser)
    {
      ArchetypeTools *archTools = componentBrowser->GetArchetypeTools();

      if (currItem)
      {
        YTE::Composition* object = currItem->GetEngineObject();

        if (object)
        {

          if (object->GetArchetypeName().Empty())
          {
            archTools->SetButtonMode(ArchetypeTools::Mode::NoArchetype);
          }
          else if (object->SameAsArchetype())
          {
            archTools->SetButtonMode(ArchetypeTools::Mode::IsSame);
          }
          else
          {
            archTools->SetButtonMode(ArchetypeTools::Mode::HasChanged);
          }

          // Load the new current object into the component browser
          ComponentTree *componentTree = componentBrowser->GetComponentTree();

          componentTree->ClearComponents();

          componentTree->LoadGameObject(object);

          YTE::Model* model = object->GetComponent<YTE::Model>();

          MaterialViewer* matViewer = mMainWindow->GetWidget<MaterialViewer>("MaterialViewer");

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
          YTE::Transform *currTransform = object->GetComponent<YTE::Transform>();

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
    }
  }

  void ObjectBrowser::OnItemSelectionChanged()
  {
    QList<QTreeWidgetItem*> items = mTree.selectedItems();

    OutputConsole* console = mMainWindow->GetWidget<OutputConsole>("OutputConsole");

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

    ObjectItem *parentItem = static_cast<ObjectItem*>(mTree.itemAt(aEvent->pos()));

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
