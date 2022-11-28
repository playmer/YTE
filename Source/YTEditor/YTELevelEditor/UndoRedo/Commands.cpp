/******************************************************************************/
/*!
\file   Commands.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the derived command classes for the undo redo system.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/ComponentWidget.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/ObjectItem.hpp"
#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"

#include "YTEditor/YTELevelEditor/UndoRedo/Commands.hpp"

#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"


namespace YTEditor
{

  AddObjectCmd::AddObjectCmd(YTE::Composition *aComposition,
                             OutputConsole *aConsole,
                             CompositionBrowser *aBrowser)
    : Command(aConsole)
    , mComposition(aComposition)
    , mObjectBrowser(aBrowser)
    , mName(aComposition->GetName())
    , mGUID(aComposition->GetGUID())
  {
    mParentGuid = aComposition->GetParent()->GetGUID();
    mSerializedComposition = aComposition->Serialize(mAllocator);
  }

  AddObjectCmd::~AddObjectCmd()
  {
  }

  void AddObjectCmd::Execute()
  {
    YTEditorMainWindow* mainWindow = mObjectBrowser->GetWorkspace<YTELevelEditor>()->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    mComposition = parentObj->AddComposition(&mSerializedComposition, mName);
    mComposition->SetGUID(mGUID);
    
    mConsole->PrintLnC(OutputConsole::Color::Green,
                       "AddObjectCmd::Execute() - Add %s",
                       mComposition->GetName().c_str());
    
    auto &name = mComposition->GetName();
    
    if (parentObj->GetType()->IsA<YTE::Space>())
    {
      mObjectBrowser->AddTreeItem(name.c_str(), mComposition, 0, true);
    }
    else
    {
      auto parent = mObjectBrowser->FindItemByComposition(parentObj);
      mObjectBrowser->AddTreeItem(name.c_str(), parent, mComposition, 0, true);
    }
  }

  void AddObjectCmd::UnExecute()
  {
    // this function should remove the object from the current level
    // it is associated with Undoing the action of adding an object
    mConsole->PrintLnC(OutputConsole::Color::Blue,
                       "AddObjectCmd::UnExecute() - Remove %s",
                       mComposition->GetName().c_str());

    auto mainWindow = mObjectBrowser->GetWorkspace<YTELevelEditor>()->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    auto item = mObjectBrowser->FindItemByComposition(mComposition);
    
    parentObj->RemoveComposition(mComposition);
    
    mObjectBrowser->RemoveObjectFromViewer(item);
  }

  RemoveObjectCmd::RemoveObjectCmd(YTE::Composition *aComposition,
                                   OutputConsole *aConsole,
                                   CompositionBrowser *aBrowser)
    : Command(aConsole)
    , mComposition(aComposition)
    , mObjectBrowser(aBrowser)
    , mName(aComposition->GetName())
    , mGUID(aComposition->GetGUID())
  {
    mParentGuid = aComposition->GetParent()->GetGUID();
    mSerializedComposition = aComposition->Serialize(mAllocator);
  }

  RemoveObjectCmd::~RemoveObjectCmd()
  {
  }

  void RemoveObjectCmd::Execute()
  {
    mConsole->PrintLnC(OutputConsole::Color::Blue,
                       "RemoveObjectCmd::Execute() - Remove %s",
                       mComposition->GetName().c_str());

    auto mainWindow = mObjectBrowser->GetWorkspace<YTELevelEditor>()->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);
    mComposition->SetGUID(mGUID);

    auto item = mObjectBrowser->FindItemByComposition(mComposition);

    parentObj->RemoveComposition(mComposition);

    mObjectBrowser->RemoveObjectFromViewer(item);
  }

  void RemoveObjectCmd::UnExecute()
  {
    auto mainWindow = mObjectBrowser->GetWorkspace<YTELevelEditor>()->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    mComposition = parentObj->AddComposition(&mSerializedComposition, mName);

    mConsole->PrintLnC(OutputConsole::Color::Green,
                       "AddObjectCmd::Execute() - Add %s",
                       mComposition->GetName().c_str());

    auto &name = mComposition->GetName();

    if (parentObj->GetType()->IsA<YTE::Space>())
    {
      mObjectBrowser->AddTreeItem(name.c_str(), mComposition, 0, true);
    }
    else
    {
      auto parent = mObjectBrowser->FindItemByComposition(parentObj);
      mObjectBrowser->AddTreeItem(name.c_str(), parent, mComposition, 0, true);
    }
  }

  AddComponentCmd::AddComponentCmd(YTE::Component* aComponent,
                                   ComponentBrowser* aBrowser,
                                   OutputConsole* aConsole)
    : Command(aConsole)
    , mComponentBrowser(aBrowser)
    , mGUID(aComponent->GetGUID())
  {
    mComponentType = aComponent->GetType();
    mParentGuid = aComponent->GetOwner()->GetGUID();
    mSerializedComponent = aComponent->Serialize(mAllocator);
  }

  AddComponentCmd::~AddComponentCmd()
  {
  }

  void AddComponentCmd::Execute()
  {
    // this function should add the component to the object
    // it is associated with Redoing the action of adding a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Add Component Command : Execute");

    auto workspace = mComponentBrowser->GetWorkspace<YTELevelEditor>();
    auto mainWindow = workspace->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    CompositionBrowser* objectBrowser = workspace->GetWidget<CompositionBrowser>();
    ObjectItem* parentItem = objectBrowser->FindItemByComposition(parentObj);
    objectBrowser->setCurrentItem(parentItem);

    ComponentTree* compTree = mComponentBrowser->GetComponentTree();
    ComponentWidget* compWidg = compTree->InternalAddComponent(mComponentType, &mSerializedComponent);
    YTE::Component* component = compWidg->GetEngineComponent();

    component->SetGUID(mGUID);
  }

  void AddComponentCmd::UnExecute()
  {
    // this function should Remove the component from the object
    // it is associated with Undoing the action of adding a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Add Component Command : UnExecute");

    ComponentTree *compTree = mComponentBrowser->GetComponentTree();
    QTreeWidgetItem *componentItem = compTree->FindComponentItem(mComponentType);
    
    compTree->BaseRemoveComponent(componentItem);
  }

  RemoveComponentCmd::RemoveComponentCmd(YTE::Component* aComponent,
                                         ComponentBrowser* aBrowser,
                                         OutputConsole* aConsole)
    : Command(aConsole)
    , mComponentBrowser(aBrowser)
    , mGUID(aComponent->GetGUID())
  {
    mComponentType = aComponent->GetType();
    mParentGuid = aComponent->GetOwner()->GetGUID();
    mSerializedComponent = aComponent->Serialize(mAllocator);
  }

  RemoveComponentCmd::~RemoveComponentCmd()
  {
  }

  void RemoveComponentCmd::Execute()
  {
    // this function should Remove the component from the object
    // it is associated with Redoing the action of removing a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Remove Component Command : Execute");

    ComponentTree *compTree = mComponentBrowser->GetComponentTree();
    QTreeWidgetItem *componentItem = compTree->FindComponentItem(mComponentType);

    compTree->BaseRemoveComponent(componentItem);
  }

  void RemoveComponentCmd::UnExecute()
  {
    // this function should add the component to the object
    // it is associated with Undoing the action of removing a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Remove Component Command : UnExecute");

    auto workspace = mComponentBrowser->GetWorkspace<YTELevelEditor>();
    auto mainWindow = workspace->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    CompositionBrowser *objBrowser = workspace->GetWidget<CompositionBrowser>();
    ObjectItem *parentItem = objBrowser->FindItemByComposition(parentObj);
    objBrowser->setCurrentItem(parentItem);

    ComponentBrowser* componentBrowser = workspace->GetWidget<ComponentBrowser>();
    ComponentTree *compTree = componentBrowser->GetComponentTree();
    ComponentWidget *compWidg = compTree->InternalAddComponent(mComponentType, &mSerializedComponent);

    YTE::Component *component = compWidg->GetEngineComponent();

    component->SetGUID(mGUID);
  }


  ChangePropValCmd::ChangePropValCmd(std::string aPropName,
                                     YTE::GlobalUniqueIdentifier aGUID,
                                     YTE::Any aOldVal,
                                     YTE::Any aNewVal,
                                     Framework::MainWindow* aMainWindow)
    : Command(aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<OutputConsole>())
    , mPropertyName(aPropName)
    , mPreviousValue(aOldVal)
    , mModifiedValue(aNewVal)
    , mMainWindow(aMainWindow)
    , mCompGUID(aGUID)
  {
    ComponentBrowser* componentBrowser = aMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ComponentBrowser>();
    mArchTools = componentBrowser->GetArchetypeTools();
  }


  ChangePropValCmd::~ChangePropValCmd()
  {
  }

  void ChangePropValCmd::Execute()
  {
    int change = mArchTools->IncrementChanges();
    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Execute: Changes = %d", change);

    YTE::Engine *engine = mMainWindow->GetWorkspace<YTELevelEditor>()->GetRunningEngine();
    YTE::Component *comp = engine->GetComponentByGUID(mCompGUID);
    YTE::Type *compType = comp->GetType();

    YTE::Property *prop = comp->GetProperty(mPropertyName, compType);

    prop->GetSetter()->Invoke(comp, mModifiedValue);
  }

  void ChangePropValCmd::UnExecute()
  {
    int change = mArchTools->DecrementChanges();

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "UnExecute: Changes = %d", change);

    YTE::Engine *engine = mMainWindow->GetWorkspace<YTELevelEditor>()->GetRunningEngine();
    YTE::Component *comp = engine->GetComponentByGUID(mCompGUID);
    YTE::Type *compType = comp->GetType();

    YTE::Property *prop = comp->GetProperty(mPropertyName, compType);

    prop->GetSetter()->Invoke(comp, mPreviousValue);
  }

  ObjectSelectionChangedCmd::ObjectSelectionChangedCmd(std::vector<YTE::GlobalUniqueIdentifier> aNewSelection,
                                                       std::vector<YTE::GlobalUniqueIdentifier> aOldSelection,
                                                       CompositionBrowser *aBrowser,
                                                       OutputConsole *aConsole)
    : Command(aConsole)
    , mObjectBrowser(aBrowser)
    , mNewSelection(aNewSelection)
    , mOldSelection(aOldSelection)
  {
  }

  void ObjectSelectionChangedCmd::Execute()
  {
    // get all the object items

    auto workspace = mObjectBrowser->GetWorkspace<YTELevelEditor>();
    auto mainWindow = workspace->GetMainWindow<YTEditorMainWindow>();
    YTE::Engine* engine = mainWindow->GetRunningEngine();


    mObjectBrowser->SetInsertSelectionChangedCommand(false);
    mObjectBrowser->clearSelection();

    for (auto guid : mNewSelection)
    {
      YTE::Composition *obj = engine->GetCompositionByGUID(guid);

      if (obj)
      {
        ObjectItem *objItem = mObjectBrowser->FindItemByComposition(obj);

        if (objItem)
        {
          mObjectBrowser->setItemSelected(objItem, true);
        }
      }
    }

    mObjectBrowser->SetInsertSelectionChangedCommand(true);
  }

  void ObjectSelectionChangedCmd::UnExecute()
  {
    // get all the object items
    YTE::Engine *engine = mObjectBrowser->GetWorkspace<YTELevelEditor>()->GetRunningEngine();


    mObjectBrowser->SetInsertSelectionChangedCommand(false);
    mObjectBrowser->clearSelection();

    for (auto guid : mOldSelection)
    {
      YTE::Composition *obj = engine->GetCompositionByGUID(guid);

      if (obj)
      {
        ObjectItem *objItem = mObjectBrowser->FindItemByComposition(obj);

        if (objItem)
        {
          mObjectBrowser->setItemSelected(objItem, true);
        }
      }
    }

    mObjectBrowser->SetInsertSelectionChangedCommand(true);
  }
}


