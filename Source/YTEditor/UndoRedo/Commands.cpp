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
#include "YTE/Core/Space.hpp"

#include "YTEditor/ComponentBrowser/ArchetypeTools.hpp"
#include "YTEditor/ObjectBrowser/ObjectItem.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"


namespace YTEditor
{

  AddObjectCmd::AddObjectCmd(YTE::Composition *aComposition,
    OutputConsole *aConsole,
    ObjectBrowser *aBrowser)
    : Command(aConsole),
    mComposition(aComposition),
    mParent(mComposition->GetParent()),
    mBrowser(aBrowser)
  {
  }

  AddObjectCmd::~AddObjectCmd()
  {
  }

  void AddObjectCmd::Execute()
  {
    // this function should add the object to the current level
    // it is associated with Redoing the action of adding an object
    mComposition = mParent->AddComposition(&mSerialization, mName);

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "AddObjectCmd::Execute() - Add %s",
      mComposition->GetName().c_str());

    auto &name = mComposition->GetName();

    if (mParent->GetType()->IsA<YTE::Space>())
    {
      mBrowser->AddTreeItem(name.c_str(), mComposition);
    }
    else
    {
      auto parent = mBrowser->FindItemByComposition(mParent);
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

    mName = mComposition->GetName();
    auto item = mBrowser->FindItemByComposition(mComposition);

    mParent = mComposition->GetParent();
    mBrowser->RemoveObjectFromViewer(item);

    mSerialization = mComposition->RemoveSerialized(mAllocator);
  }

  RemoveObjectCmd::RemoveObjectCmd(const char *aObjName, OutputConsole *aConsole)
    : Command(aConsole),
    mObjectName(aObjName)
  {
  }

  RemoveObjectCmd::~RemoveObjectCmd()
  {
  }

  void RemoveObjectCmd::Execute()
  {
    // this function should remove the object from the current level
    // it is associated with Redoing the action of removing an object

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "RemoveObjectCmd::Execute() - Remove %s",
      mObjectName.c_str());
  }

  void RemoveObjectCmd::UnExecute()
  {
    // this function should add the object to the current level
    // it is associated with Undoing the action of removing an object


    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "RemoveObjectCmd::UnExecute() - Add %s",
      mObjectName.c_str());
  }

  AddComponentCmd::AddComponentCmd(const char *aCompName, OutputConsole *aConsole)
    : Command(aConsole)
  {
    (void)aCompName;
    (void)aConsole;
    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Add Component Command : Constructed");
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
  }

  void AddComponentCmd::UnExecute()
  {
    // this function should Remove the component from the object
    // it is associated with Undoing the action of adding a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Add Component Command : UnExecute");
  }

  RemoveComponentCmd::RemoveComponentCmd(const char *aCompName, OutputConsole *aConsole)
    : Command(aConsole)
  {
    (void)aCompName;
    (void)aConsole;
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
  }

  void RemoveComponentCmd::UnExecute()
  {
    // this function should add the component to the object
    // it is associated with Undoing the action of removing a component

    mConsole->PrintLnC(OutputConsole::Color::Blue,
      "Remove Component Command : UnExecute");
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


