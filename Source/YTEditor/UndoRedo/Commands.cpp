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


#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/ComponentBrowser/ComponentBrowser.hpp"
#include "YTEditor/ComponentBrowser/ComponentTree.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{

  AddObjectCmd::AddObjectCmd(YTE::Composition *aComposition,
                             OutputConsole *aConsole,
                             ObjectBrowser *aBrowser)
    : Command(aConsole)
    , mComposition(aComposition)
    , mBrowser(aBrowser)
    , mName(aComposition->GetName())
  {
    mParentGuid = aComposition->GetParent()->GetGUID();
    mSerializedComposition = aComposition->Serialize(mAllocator);
  }

  AddObjectCmd::~AddObjectCmd()
  {
  }

  void AddObjectCmd::Execute()
  {
    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    mComposition = parentObj->AddComposition(&mSerializedComposition, mName);
    
    mConsole->PrintLnC(OutputConsole::Color::Green,
                       "AddObjectCmd::Execute() - Add %s",
                       mComposition->GetName().c_str());
    
    auto &name = mComposition->GetName();
    
    if (parentObj->GetType()->IsA<YTE::Space>())
    {
      mBrowser->AddTreeItem(name.c_str(), mComposition);
    }
    else
    {
      auto parent = mBrowser->FindItemByComposition(parentObj);
      mBrowser->AddTreeItem(name.c_str(), parent, mComposition);
    }
  }

  void AddObjectCmd::UnExecute()
  {
    // this function should remove the object from the current level
    // it is associated with Undoing the action of adding an object
    mConsole->PrintLnC(OutputConsole::Color::Blue,
                       "AddObjectCmd::UnExecute() - Remove %s",
                       mComposition->GetName().c_str());

    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    auto item = mBrowser->FindItemByComposition(mComposition);
    
    parentObj->RemoveComposition(mComposition);
    
    mBrowser->RemoveObjectFromViewer(item);
  }

  RemoveObjectCmd::RemoveObjectCmd(YTE::Composition *aComposition,
                                   OutputConsole *aConsole,
                                   ObjectBrowser *aBrowser)
    : Command(aConsole)
    , mComposition(aComposition)
    , mBrowser(aBrowser)
    , mName(aComposition->GetName())
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

    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    auto item = mBrowser->FindItemByComposition(mComposition);

    parentObj->RemoveComposition(mComposition);

    mBrowser->RemoveObjectFromViewer(item);
  }

  void RemoveObjectCmd::UnExecute()
  {
    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    mComposition = parentObj->AddComposition(&mSerializedComposition, mName);

    mConsole->PrintLnC(OutputConsole::Color::Green,
                       "AddObjectCmd::Execute() - Add %s",
                       mComposition->GetName().c_str());

    auto &name = mComposition->GetName();

    if (parentObj->GetType()->IsA<YTE::Space>())
    {
      mBrowser->AddTreeItem(name.c_str(), mComposition);
    }
    else
    {
      auto parent = mBrowser->FindItemByComposition(parentObj);
      mBrowser->AddTreeItem(name.c_str(), parent, mComposition);
    }
  }

  AddComponentCmd::AddComponentCmd(YTE::Component *aComponent,
    ComponentBrowser *aBrowser,
    OutputConsole *aConsole)
    : Command(aConsole)
    , mBrowser(aBrowser)
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

    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    ObjectBrowser &objBrowser = mainWindow->GetObjectBrowser();
    ObjectItem *parentItem = objBrowser.FindItemByComposition(parentObj);
    objBrowser.setCurrentItem(parentItem);

    mainWindow->GetComponentBrowser().GetComponentTree()->InternalAddComponent(mComponentType, &mSerializedComponent);
  }

  void AddComponentCmd::UnExecute()
  {
    // this function should Remove the component from the object
    // it is associated with Undoing the action of adding a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Add Component Command : UnExecute");

    ComponentTree *compTree = mBrowser->GetComponentTree();
    QTreeWidgetItem *componentItem = compTree->FindComponentItem(mComponentType);
    
    compTree->BaseRemoveComponent(componentItem);
  }

  RemoveComponentCmd::RemoveComponentCmd(YTE::Component *aComponent,
                                         ComponentBrowser *aBrowser,
                                         OutputConsole *aConsole)
    : Command(aConsole)
    , mBrowser(aBrowser)
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

    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    parentObj->RemoveComponent(mComponentType);

    mBrowser->GetComponentTree()->ClearComponents();
    mBrowser->GetComponentTree()->LoadGameObject(parentObj);
  }

  void RemoveComponentCmd::UnExecute()
  {
    // this function should add the component to the object
    // it is associated with Undoing the action of removing a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Remove Component Command : UnExecute");

    MainWindow *mainWindow = mBrowser->GetMainWindow();
    YTE::Engine *engine = mainWindow->GetRunningEngine();
    YTE::Composition *parentObj = engine->GetCompositionByGUID(mParentGuid);

    mComponent = parentObj->AddComponent(mComponentType, &mSerializedComponent);

    ObjectBrowser &objBrowser = mainWindow->GetObjectBrowser();
    ObjectItem *parentItem = objBrowser.FindItemByComposition(parentObj);
    objBrowser.setCurrentItem(parentItem);

    // reload all components on the object
    mBrowser->GetComponentTree()->ClearComponents();
    mBrowser->GetComponentTree()->LoadGameObject(parentObj);
  }


  ChangePropValCmd::ChangePropValCmd(YTE::Type *aCmpType,
    YTE::Any aOldVal,
    YTE::Any aNewVal,
    OutputConsole *aConsole,
    ArchetypeTools *aTools)
    : Command(aConsole),
    mPreviousValue(aOldVal),
    mModifiedValue(aNewVal),
    mCmpType(aCmpType),
    mConsole(aConsole),
    mArchTools(aTools)
  {

  }


  ChangePropValCmd::~ChangePropValCmd()
  {
  }

  void ChangePropValCmd::Execute()
  {
    int change = mArchTools->IncrementChanges();
    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Execute: Changes = %d", change);
  }

  void ChangePropValCmd::UnExecute()
  {
    int change = mArchTools->DecrementChanges();
    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "UnExecute: Changes = %d", change);
  }

}


