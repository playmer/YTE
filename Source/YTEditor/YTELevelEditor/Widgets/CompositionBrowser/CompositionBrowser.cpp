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
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectItem.hpp"
#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/Commands.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  CompositionBrowser::CompositionBrowser(YTELevelEditor* aEditor)
    : Widget{ aEditor }
    , mLevelEditor{ aEditor }
  {
    SetWidgetSettings();

    connect(this, &QTreeWidget::customContextMenuRequested,
      this, &CompositionBrowser::CreateContextMenu);

    // Calls OnCurrentItemChanged() when the currentItemChanged event is received
    connect(this, &QTreeWidget::currentItemChanged,
      this, &CompositionBrowser::OnCurrentItemChanged);

    // item selection for undo redo
    connect(this, &QTreeWidget::itemSelectionChanged,
      this, &CompositionBrowser::OnItemSelectionChanged);

    connect(this, &QTreeWidget::itemChanged,
      this, &CompositionBrowser::OnItemTextChanged);

    setItemDelegate(new ObjectItemDelegate(this));

    setMinimumSize(200, 200);
  }

  CompositionBrowser::~CompositionBrowser()
  {
  }

  void CompositionBrowser::ClearObjectList()
  {
    clear();
  }

  ObjectItem* CompositionBrowser::AddObject(const char *aCompositionName,
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

    return AddTreeItem(aCompositionName, composition, aIndex, true);
  }

  ObjectItem* CompositionBrowser::AddExistingComposition(const char *aCompositionName,
    YTE::Composition *aComposition)
  {
    return AddTreeItem(aCompositionName, aComposition, 0, true);
  }

  ObjectItem* CompositionBrowser::AddChildObject(const char *aCompositionName,
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

    return AddTreeItem(aCompositionName, aParentObj, composition, aIndex, true);
  }

  ObjectItem* CompositionBrowser::AddTreeItem(const char *aItemName,
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

    ObjectItem *item = new ObjectItem(name, this, aEngineObj, space);

    // Add new item as a top level member in the tree hierarchy
    // (object should have no parent objects)
    insertTopLevelItem(aIndex, item);

    if (aSetAsCurrent)
    {
      setCurrentItem(item);
    }


    for (auto const& [compositionName, child] : aEngineObj->GetCompositions())
    {
      AddTreeItem(child->GetName().c_str(), item, child.get(), 0, aSetAsCurrent);
    }

    return item;
  }

  ObjectItem* CompositionBrowser::AddTreeItem(const char *aItemName,
    ObjectItem *aParentObj,
    YTE::Composition *aEngineObj,
    int aIndex,
    bool aSetAsCurrent)
  {
    YTE::String name{ aItemName };

    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
    
    auto space = editor->GetEditingLevel();

    ObjectItem* item = new ObjectItem{ name, aParentObj, aEngineObj, space };

    // add this object as a child of another tree item
    aParentObj->insertChild(aIndex, item);

    if (aSetAsCurrent)
    {
      setCurrentItem(item);
    }

    for (auto const& [compositionName, child] : aEngineObj->GetCompositions())
    {
      AddTreeItem(child->GetName().c_str(), item, child.get(), 0, aSetAsCurrent);
    }

    return item;
  }


  void CompositionBrowser::LoadAllChildObjects(YTE::Composition* aParentObj, ObjectItem* aParentItem)
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

  void CompositionBrowser::SetWidgetSettings()
  {
    setObjectName("Composition Browser");

    setContextMenuPolicy(Qt::CustomContextMenu);
    setDragDropMode(QAbstractItemView::InternalMove);
    setMouseTracking(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
  }

  void CompositionBrowser::CreateContextMenu(const QPoint& pos)
  {
    QTreeWidgetItem* item = itemAt(pos);

    if (item == nullptr)
    {
      return;
    }

    QMenu* contextMenu = new QMenu(this);

    QAction* removeAct = new QAction("Remove", contextMenu);
    connect(removeAct, &QAction::triggered, this, &CompositionBrowser::RemoveCurrentObject);

    contextMenu->addAction(removeAct);
    contextMenu->exec(this->mapToGlobal(pos));
  }


  void CompositionBrowser::DuplicateCurrentlySelected()
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

  void CompositionBrowser::dropEvent(QDropEvent *aEvent)
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

  void CompositionBrowser::RemoveCurrentObject()
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
    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();

    auto name = currItem->text(0).toStdString();
    auto cmd = std::make_unique<RemoveObjectCmd>(engineObj,
                                                 editor->GetWidget<OutputConsole>(),
                                                 editor->GetWidget<CompositionBrowser>());

    editor->GetUndoRedo()->InsertCommand(std::move(cmd));

    // remove current object from engine
    currItem->DeleteFromEngine();

    RemoveObjectFromViewer(currItem);
  }

  void CompositionBrowser::SetInsertSelectionChangedCommand(bool isActive)
  {
    mInsertSelectionChangedCmd = isActive;
  }

  void CompositionBrowser::MoveToFrontOfCamera(YTE::Composition *aObject)
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

  std::string CompositionBrowser::GetName()
  {
    return "ObjectBrowser";
  }

  ToolWindowManager::AreaReference CompositionBrowser::GetToolArea()
  {
    auto workspace = GetWorkspace<YTELevelEditor>();
    auto area = mWorkspace->GetMainWindow()->GetToolWindowManager()->areaOf(workspace->GetLevelWindowWidget());

    return ToolWindowManager::AreaReference{
      ToolWindowManager::AreaReferenceType::LeftWindowSide,
      area
    };
  }

  void CompositionBrowser::RemoveObjectFromViewer(ObjectItem *aItem)
  {
    // clear the component viewer
    YTELevelEditor* editor = GetWorkspace<YTELevelEditor>();
    editor->GetWidget<ComponentBrowser>()->GetComponentTree()->ClearComponents();

    // hide and remove from the tree
    aItem->setHidden(true);

    auto parent = aItem->parent();

    if (nullptr == parent)
    {
      int index = indexOfTopLevelItem(aItem);
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

    auto matViewer = editor->GetWidget<MaterialViewer>();

    if (matViewer && currItem && currItem->GetEngineObject())
    {
      YTE::Model *model = currItem->GetEngineObject()->GetComponent<YTE::Model>();

      if (model)
      {
        YTE::Mesh *mesh = model->GetMesh();

        if (mesh)
        {
          // get the list of materials from the submeshes
          auto& submeshes = mesh->mParts;

          matViewer->LoadMaterial(submeshes[0].mData.mUBOMaterial);

          matViewer->SetMaterialsList(&submeshes);
        }
      }
    }

    if (topLevelItemCount() == 0)
    {
      editor->GetGizmoToolbar()->SetMode(GizmoToolbar::Mode::Select);
    }
  }

  void CompositionBrowser::keyPressEvent(QKeyEvent *aEvent)
  {
    if (aEvent->key() == Qt::Key_Delete)
    {
      RemoveCurrentObject();
    }
    else
    {
      // TODO: just enable this
      //QTreeWidget::keyPressEvent(aEvent);
    }
  }

  YTE::Composition* CompositionBrowser::GetCurrentObject()
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

  void CompositionBrowser::setCurrentItem(ObjectItem* aItem)
  {
    QTreeWidget::setCurrentItem(aItem);
  }

  void CompositionBrowser::setCurrentItem(ObjectItem* aItem, int aColumn)
  {
    QTreeWidget::setCurrentItem(aItem, aColumn);
  }


  void CompositionBrowser::setItemSelected(ObjectItem* aItem, bool aSelected)
  {
    QTreeWidget::setItemSelected(aItem, aSelected);
  }


  void CompositionBrowser::clearSelection()
  {
    QTreeWidget::clearSelection();
  }

  int CompositionBrowser::indexOfTopLevelItem(ObjectItem* aObject)
  {
    return QTreeWidget::indexOfTopLevelItem(aObject);
  }

  ObjectItem* CompositionBrowser::topLevelItem(int index)
  {
    return static_cast<ObjectItem*>(QTreeWidget::topLevelItem(index));
  }

  void CompositionBrowser::setHeaderLabel(char const* aLabel)
  {
    QTreeWidget::setHeaderLabel(aLabel);
  }


  ObjectItem* CompositionBrowser::SearchChildrenByComp(ObjectItem *aItem, YTE::Composition *aComp)
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

  void CompositionBrowser::FindObjectsByArchetypeInternal(YTE::String &archetypeName,
                                                     std::vector<ObjectItem*>& result,
                                                     ObjectItem* item)
  {
    for (int i = 0; i < item->childCount(); ++i)
    {
      if (item->GetEngineObject()->GetArchetypeName() == archetypeName)
      {
        result.emplace_back(item);
      }

      FindObjectsByArchetypeInternal(archetypeName, result, item);
    }
  }

  ObjectItem* CompositionBrowser::FindItemByComposition(YTE::Composition *aComp)
  {
    for (int i = 0; i < topLevelItemCount(); ++i)
    {
      ObjectItem *item = topLevelItem(i);

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

  std::vector<ObjectItem*> CompositionBrowser::FindAllObjectsOfArchetype(YTE::String &aArchetypeName)
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

    return result;
  }

  void CompositionBrowser::SelectNoItem()
  {
    setCurrentItem(nullptr);
  }

  void CompositionBrowser::OnCurrentItemChanged(QTreeWidgetItem* aCurrent,
    QTreeWidgetItem* aPrevious)
  {
    YTE::UnusedArguments(aPrevious);

    //ObjectItem *prevObj = aPrevious ? static_cast<ObjectItem*>(aPrevious) : nullptr;
    ObjectItem* currObj = aCurrent ? static_cast<ObjectItem*>(aCurrent) : nullptr;

    ArchetypeTools* archTools = mLevelEditor->GetWidget<ComponentBrowser>()->GetArchetypeTools();

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
      ComponentBrowser* componentBrowser = mLevelEditor->GetWidget<ComponentBrowser>();
      ComponentTree* componentTree = componentBrowser->GetComponentTree();
      componentTree->ClearComponents();
      componentTree->LoadGameObject(currObj->GetEngineObject());

      YTE::Model* model = currObj->GetEngineObject()->GetComponent<YTE::Model>();

      auto matViewer = mLevelEditor->GetWidget<MaterialViewer>();

      if (matViewer && model && model->GetMesh())
      {
        // get the list of materials from the submeshes
        auto& submeshes = model->GetMesh()->mParts;

        matViewer->LoadMaterial(submeshes[0].mData.mUBOMaterial);
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
      YTE::Transform* currTransform = currObj->GetEngineObject()->GetComponent<YTE::Transform>();

      if (currTransform)
      {
        Gizmo* giz = mLevelEditor->GetGizmo();

        if (giz)
        {
          giz->SnapToCurrentObject();
        }
      }
    }
  }

  void CompositionBrowser::OnItemSelectionChanged()
  {
    QList<QTreeWidgetItem*> items = selectedItems();

    auto console = mLevelEditor->GetWidget<OutputConsole>();
    auto browser = mLevelEditor->GetWidget<CompositionBrowser>();

    std::vector<YTE::GlobalUniqueIdentifier> newSelection;
    std::vector<YTE::GlobalUniqueIdentifier> oldSelection;

    for (auto item : items)
    {
      ObjectItem* objItem = static_cast<ObjectItem*>(item);

      auto guid = objItem->GetEngineObject()->GetGUID();

      newSelection.push_back(guid);
    }

    for (auto item : mSelectedItems)
    {
      oldSelection.push_back(item);
    }

    if (mInsertSelectionChangedCmd)
    {
      UndoRedo* undoRedo = mLevelEditor->GetUndoRedo();
      undoRedo->InsertCommand(std::make_unique<ObjectSelectionChangedCmd>(newSelection, oldSelection, browser, console));
    }

    mSelectedItems = newSelection;
  }


  void CompositionBrowser::OnItemTextChanged(QTreeWidgetItem* aItem, int aIndex)
  {
    (void)aIndex;

    ObjectItem* currItem = static_cast<ObjectItem*>(aItem);

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

}