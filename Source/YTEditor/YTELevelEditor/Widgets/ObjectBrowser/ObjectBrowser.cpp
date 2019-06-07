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

#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/Orientation.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/MaterialViewer/MaterialViewer.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/Commands.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  ObjectBrowser::ObjectBrowser(YTELevelEditor* editor)
    : Widget(editor)
    , mTree()
  {
    SetWidgetSettings();

    // TODO(NICK): NO MORE OBJECT TREE :(
    //mTree.setItemDelegate(new ObjectItemDelegate(this));
  }

  ObjectBrowser::~ObjectBrowser()
  {
  }

  void ObjectBrowser::ClearObjectList()
  {
    mTree->clear();
  }

  ObjectItem* ObjectBrowser::AddObject(const char *aCompositionName,
    const char *aArchetypeName,
    int aIndex)
  {
    if (std::string(aArchetypeName) == "Gizmo")
    {
      return nullptr;
    }

    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();

    auto space = editor->GetEditingLevel();

    auto composition = space->AddComposition(aArchetypeName, aCompositionName);

    OutputConsole* console = editor->GetWidget<OutputConsole>();

    auto cmd = std::make_unique<AddObjectCmd>(composition, console, this);

    editor->GetUndoRedo()->InsertCommand(std::move(cmd));

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
    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();

    auto& spaces = editor->GetRunningEngine()->GetCompositions();
    auto space = spaces.begin()->second.get();

    auto composition = space->AddComposition(aArchetypeName, aCompositionName);

    OutputConsole* console = editor->GetWidget<OutputConsole>();

    auto cmd = std::make_unique<AddObjectCmd>(composition, console, this);

    editor->GetUndoRedo()->InsertCommand(std::move(cmd));

    return AddTreeItem(aCompositionName, aParentObj, composition, aIndex);
  }

  ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName,
    YTE::Composition *aEngineObj,
    int aIndex,
    bool aSetAsCurrent)
  {
    YTE::String name{ aItemName };

    if (std::string(aItemName) == "Gizmo")
    {
      return nullptr;
    }

    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();

    YTE::Composition *space = editor->GetEditingLevel();

    // TODO(NICK): NO MORE OBJECT TREE :(
    ObjectItem *item = new ObjectItem(name, mTree, aEngineObj, space);

    // Add new item as a top level member in the tree hierarchy
    // (object should have no parent objects)
    mTree->insertTopLevelItem(aIndex, item);

    if (aSetAsCurrent)
    {
      mTree->setCurrentItem(item);
    }

    auto& compMap = aEngineObj->GetCompositions();

    for (auto iter = compMap.begin(); iter != compMap.end(); iter++)
    {
      YTE::Composition *child = iter->second.get();

      AddTreeItem(child->GetName().c_str(), item, child);
    }

    return item;
  }

  ObjectItem* ObjectBrowser::AddTreeItem(const char *aItemName,
    ObjectItem *aParentObj,
    YTE::Composition *aEngineObj,
    int aIndex,
    bool aSetAsCurrent)
  {
    YTE::String name{ aItemName };

    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
    
    auto space = editor->GetEditingLevel();

    ObjectItem *item = new ObjectItem(name, aParentObj, aEngineObj, space);

    // add this object as a child of another tree item
    aParentObj->insertChild(aIndex, item);

    if (aSetAsCurrent)
    {
      mTree->setCurrentItem(item);
    }

    auto& compMap = aEngineObj->GetCompositions();

    for (auto iter = compMap.begin(); iter != compMap.end(); iter++)
    {
      YTE::Composition *child = iter->second.get();

      AddTreeItem(child->GetName().c_str(), item, child);
    }

    return item;
  }


  void ObjectBrowser::LoadAllChildObjects(YTE::Composition* aParentObj, ObjectItem* aParentItem)
  {
    // if the parent object has no children
    if (aParentObj->GetCompositions().size() == 0)
    {
      return;
    }

    for (auto& cmp : aParentObj->GetCompositions())
    {
      ObjectItem* item = AddTreeItem(cmp.first.c_str(), aParentItem, cmp.second.get(), 0, false);

      if (item)
      {
        LoadAllChildObjects(cmp.second.get(), item);
      }
    }
  }

  void ObjectBrowser::SetWidgetSettings()
  {
    setObjectName("ObjectBrowser");
    setMinimumWidth(200);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);

    mTree->setDragDropMode(QAbstractItemView::InternalMove);
    mTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
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

    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
    YTE::Composition *duplicate = editor->GetEditingLevel()->AddComposition(&serialized, "Copy");

    YTE::String guid = duplicate->GetGUID().ToString();
    duplicate->SetName(guid);

    AddExistingComposition(guid.c_str(), duplicate);
  }

  void ObjectBrowser::dropEvent(QDropEvent *aEvent)
  {
    YTE::Composition *movedObj = GetCurrentObject();

    ObjectItem *parentItem = static_cast<ObjectItem*>(mTree->itemAt(aEvent->pos()));

    YTE::Composition *parentObj = nullptr;

    if (parentItem)
    {
      parentObj = parentItem->GetEngineObject();
    }

    movedObj->ReParent(parentObj);

    // TODO(NICK): figure out how to pass this to a tree, might need to inherit from QTreeWidget
    //dropEvent(aEvent);
  }

  void ObjectBrowser::RemoveCurrentObject()
  {
    if (mTree->topLevelItemCount() == 0)
    {
      return;
    }

    ObjectItem *currItem = dynamic_cast<ObjectItem*>(mTree->currentItem());

    if (!currItem || !currItem->GetEngineObject())
    {
      return;
    }

    YTE::Composition *engineObj = currItem->GetEngineObject();
    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();

    auto name = currItem->text(0).toStdString();
    auto cmd = std::make_unique<RemoveObjectCmd>(engineObj,
                                                 editor->GetWidget<OutputConsole>(),
                                                 editor->GetWidget<ObjectBrowser>());

    editor->GetUndoRedo()->InsertCommand(std::move(cmd));

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
      YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
      YTE::Composition *camera = editor->GetEditorCamera();

      YTE::Transform *camTransform = camera->GetComponent<YTE::Transform>();
      YTE::Orientation *orientation = camera->GetComponent<YTE::Orientation>();

      if (camTransform && orientation)
      {
        glm::vec3 newPos = camTransform->GetWorldTranslation() - 10.0f * orientation->GetForwardVector();

        transform->SetWorldTranslation(newPos);
      }
    }
  }

  std::string ObjectBrowser::GetName()
  {
    return "ObjectBrowser";
  }

  Framework::Widget::DockArea ObjectBrowser::GetDefaultDockArea() const
  {
    return Widget::DockArea::Left;
  }

  void ObjectBrowser::RemoveObjectFromViewer(ObjectItem *aItem)
  {
    // clear the component viewer
    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
    editor->GetWidget<ComponentBrowser>()->GetComponentTree()->ClearComponents();

    // hide and remove from the tree
    aItem->setHidden(true);

    auto parent = aItem->parent();

    if (nullptr == parent)
    {
      int index = mTree->indexOfTopLevelItem(aItem);
      auto item = mTree->takeTopLevelItem(index);
      delete item;
    }
    else
    {
      int index = parent->indexOfChild(aItem);
      auto item = parent->takeChild(index);
      delete item;
    }

    mTree->setCurrentItem(mTree->topLevelItem(0));

    ObjectItem *currItem = dynamic_cast<ObjectItem*>(mTree->currentItem());

    auto matViewer = editor->GetWidget<MaterialViewer>();

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

    if (mTree->topLevelItemCount() == 0)
    {
      editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Select);
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
      // TODO(NICK): figure out how to pass this to a tree, might need to inherit from QTreeWidget
      //mTree.keyPressEvent(aEvent);
    }
  }

  YTE::Composition* ObjectBrowser::GetCurrentObject()
  {
    auto objItem = dynamic_cast<ObjectItem*>(mTree->currentItem());

    if (objItem)
    {
      return objItem->GetEngineObject();
    }
    else
    {
      return nullptr;
    }

  }

  void ObjectBrowser::setCurrentItem(ObjectItem* aItem)
  {
    mTree->setCurrentItem(aItem);
  }

  void ObjectBrowser::setCurrentItem(ObjectItem* aItem, int aColumn)
  {
    mTree->setCurrentItem(aItem, aColumn);
  }


  void ObjectBrowser::setItemSelected(ObjectItem* aItem, bool aSelected)
  {
    mTree->setItemSelected(aItem, aSelected);
  }


  void ObjectBrowser::clearSelection()
  {
    mTree->clearSelection();
  }

  int ObjectBrowser::indexOfTopLevelItem(ObjectItem* aObject)
  {
    return mTree->indexOfTopLevelItem(aObject);
  }

  ObjectItem* ObjectBrowser::topLevelItem(int index)
  {
    return static_cast<ObjectItem*>(mTree->topLevelItem(index));
  }

  void ObjectBrowser::setHeaderLabel(char const* aLabel)
  {
    mTree->setHeaderLabel(aLabel);
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

  void ObjectBrowser::FindObjectsByArchetypeInternal(YTE::String &archetypeName,
                                                     std::vector<ObjectItem*>& result,
                                                     ObjectItem* item)
  {
    for (int i = 0; i < item->childCount(); ++i)
    {
      ObjectItem* currentItem = dynamic_cast<ObjectItem*>(item->child(i));

      if (item->GetEngineObject()->GetArchetypeName() == archetypeName)
      {
        result.emplace_back(item);
      }

      FindObjectsByArchetypeInternal(archetypeName, result, item);
    }
  }

  ObjectItem* ObjectBrowser::FindItemByComposition(YTE::Composition *aComp)
  {
    for (int i = 0; i < mTree->topLevelItemCount(); ++i)
    {
      ObjectItem *item = dynamic_cast<ObjectItem*>(mTree->topLevelItem(i));

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

  std::vector<ObjectItem*> ObjectBrowser::FindAllObjectsOfArchetype(YTE::String &aArchetypeName)
  {
    std::vector<ObjectItem*> result;

    // loop through all items in the object browser
    for (int i = 0; i < mTree->topLevelItemCount(); ++i)
    {
      ObjectItem *objItem = dynamic_cast<ObjectItem*>(mTree->topLevelItem(i));

      if (objItem->GetEngineObject()->GetArchetypeName() == aArchetypeName)
      {
        result.emplace_back(objItem);
      }

      FindObjectsByArchetypeInternal(aArchetypeName, result, objItem);
    }

    return result;
  }

  void ObjectBrowser::SelectNoItem()
  {
    mTree->setCurrentItem(nullptr);
  }

}