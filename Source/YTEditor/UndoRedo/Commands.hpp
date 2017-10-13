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

#include "YTE/Core/Utilities.hpp"
#include "YTE/Meta/Type.hpp"

#include "UndoRedo.hpp"

class ObjectItem;
class ObjectBrowser;
class ArchetypeTools;

namespace YTE
{
  class Composition;
}

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
  YTE::Composition *mParent;
  ObjectBrowser *mBrowser;
  YTE::RSValue mSerialization;
  YTE::RSAllocator mAllocator;
  YTE::String mName;
};

class RemoveObjectCmd : public Command
{
public:
  RemoveObjectCmd(const char *aObjName, OutputConsole *aConsole);
  ~RemoveObjectCmd();

  void Execute() override;
  void UnExecute() override;

private:
  std::string mObjectName;
};

class AddComponentCmd : public Command
{
public:
  AddComponentCmd(const char *aCompName, OutputConsole *aConsole);
  ~AddComponentCmd();

  void Execute() override;
  void UnExecute() override;

private:
};

class RemoveComponentCmd : public Command
{
public:
  RemoveComponentCmd(const char *aCompName, OutputConsole *aConsole);
  ~RemoveComponentCmd();

  void Execute() override;
  void UnExecute() override;

private:
};


class ChangePropValCmd : public Command
{
public:
  ChangePropValCmd(YTE::Type *aCmpType, 
                   YTE::Any *aOldVal, 
                   YTE::Any *aNewVal, 
                   OutputConsole *aConsole, 
                   ArchetypeTools *aTools);

  ~ChangePropValCmd();

  void Execute() override;
  void UnExecute() override;

private:
  YTE::Any *mPreviousValue;
  YTE::Any *mModifiedValue;
  YTE::Type *mCmpType;

  OutputConsole *mConsole;
  ArchetypeTools *mArchTools;

};
