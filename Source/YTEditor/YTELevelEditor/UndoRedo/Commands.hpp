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

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Meta/Meta.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"
#include "YTEditor/YTELevelEditor/UndoRedo/UndoRedo.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

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
    ObjectBrowser *mObjectBrowser;
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
    ObjectBrowser *mObjectBrowser;
    YTE::RSValue mSerializedComposition;
    YTE::RSAllocator mAllocator;
    YTE::String mName;
    YTE::GlobalUniqueIdentifier mGUID;
  };

  class AddComponentCmd : public Command
  {
  public:
    AddComponentCmd(YTE::Component* aComponent,
                    ComponentBrowser* aBrowser,
                    OutputConsole* aConsole);
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

    ComponentBrowser *mComponentBrowser;
  };

  class RemoveComponentCmd : public Command
  {
  public:
    RemoveComponentCmd(YTE::Component* aComponent,
                       ComponentBrowser* aBrowser,
                       OutputConsole* aConsole);
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

    ComponentBrowser *mComponentBrowser;

  };


  class ChangePropValCmd : public Command
  {
  public:
    ChangePropValCmd(std::string aPropName,
                     YTE::GlobalUniqueIdentifier aGUID,
                     YTE::Any aOldVal,
                     YTE::Any aNewVal,
                     Framework::MainWindow* aMainWindow);
    ~ChangePropValCmd();
    void Execute() override;
    void UnExecute() override;
  private:
    std::string mPropertyName;
    YTE::Any mPreviousValue;
    YTE::Any mModifiedValue;
    YTE::GlobalUniqueIdentifier mCompGUID;
    ArchetypeTools* mArchTools;
    Framework::MainWindow* mMainWindow;
  };
  
  class ObjectSelectionChangedCmd : public Command
  {
  public:
    ObjectSelectionChangedCmd(std::vector<YTE::GlobalUniqueIdentifier> aNewSelection,
                              std::vector<YTE::GlobalUniqueIdentifier> aOldSelection,
                              ObjectBrowser* aBrowser,
                              OutputConsole* aConsole);
    void Execute() override;
    void UnExecute() override;
  private:
    ObjectBrowser* mObjectBrowser;
    std::vector<YTE::GlobalUniqueIdentifier> mNewSelection;
    std::vector<YTE::GlobalUniqueIdentifier> mOldSelection;
  };
}
