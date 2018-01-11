/******************************************************************************/
/*!
\file   Commands.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The classes deriving from Command -> specific commands that can be undone.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Utilities/Utilities.h"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Meta/Type.hpp"

#include "YTEditor/OutputConsole/OutputConsole.hpp"
#include "YTEditor/UndoRedo/UndoRedo.hpp"


namespace YTE
{
  class Composition;
}

namespace YTEditor
{

  class ObjectItem;
  class ObjectBrowser;
  class ArchetypeTools;


  class AddObjectCmd : public Command
  {
  public:
    AddObjectCmd(YTE::Composition *aObject,
                 OutputConsole *aConsole,
                 ObjectBrowser *aBrowser);
    ~AddObjectCmd();

    void Execute() override;
    void UnExecute() override;

  private:
    YTE::Composition *mComposition;
    YTE::GlobalUniqueIdentifier mParentGuid;
    ObjectBrowser *mBrowser;
    YTE::RSValue mSerializedComposition;
    YTE::RSAllocator mAllocator;
    YTE::String mName;
    YTE::GlobalUniqueIdentifier mGUID;
  };

  class RemoveObjectCmd : public Command
  {
  public:
    RemoveObjectCmd(YTE::Composition *aComposition,
                    OutputConsole *aConsole,
                    ObjectBrowser *aBrowser);
    ~RemoveObjectCmd();

    void Execute() override;
    void UnExecute() override;

  private:
    YTE::Composition *mComposition;
    YTE::GlobalUniqueIdentifier mParentGuid;
    ObjectBrowser *mBrowser;
    YTE::RSValue mSerializedComposition;
    YTE::RSAllocator mAllocator;
    YTE::String mName;
    YTE::GlobalUniqueIdentifier mGUID;
  };

  class AddComponentCmd : public Command
  {
  public:
    AddComponentCmd(YTE::Component *aComponent,
                    ComponentBrowser *aBrowser,
                    OutputConsole *aConsole);
    ~AddComponentCmd();

    void Execute() override;
    void UnExecute() override;

  private:
    YTE::GlobalUniqueIdentifier mParentGuid;
    YTE::Component *mComponent;
    YTE::Type *mComponentType;
    YTE::RSValue mSerializedComponent;
    YTE::RSAllocator mAllocator;
    YTE::GlobalUniqueIdentifier mGUID;

    ComponentBrowser *mBrowser;
  };

  class RemoveComponentCmd : public Command
  {
  public:
    RemoveComponentCmd(YTE::Component *aComponent,
                       ComponentBrowser *aBrowser,
                       OutputConsole *aConsole);
    ~RemoveComponentCmd();

    void Execute() override;
    void UnExecute() override;

  private:
    YTE::GlobalUniqueIdentifier mParentGuid;
    YTE::Component *mComponent;
    YTE::Type *mComponentType;
    YTE::RSValue mSerializedComponent;
    YTE::RSAllocator mAllocator;
    YTE::GlobalUniqueIdentifier mGUID;

    ComponentBrowser *mBrowser;

  };


  class ChangePropValCmd : public Command
  {
  public:
    ChangePropValCmd(YTE::Type *aCmpType,
      YTE::Any aOldVal,
      YTE::Any aNewVal,
      OutputConsole *aConsole,
      ArchetypeTools *aTools);

    ~ChangePropValCmd();

    void Execute() override;
    void UnExecute() override;

  private:
    YTE::Any mPreviousValue;
    YTE::Any mModifiedValue;
    YTE::Type *mCmpType;

    OutputConsole *mConsole;
    ArchetypeTools *mArchTools;

  };

}
