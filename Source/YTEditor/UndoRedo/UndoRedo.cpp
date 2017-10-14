/******************************************************************************/
/*!
\file   UndoRedo.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the undo redo system for the application.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "UndoRedo.hpp"

UndoRedo::UndoRedo() : mUndoStack(), mRedoStack()
{
}

UndoRedo::~UndoRedo()
{
}

void UndoRedo::ExecuteUndo()
{
  if (mUndoStack.size() == 0)
  {
    return;
  }

  mUndoStack.top()->UnExecute();
  mRedoStack.emplace(std::move(mUndoStack.top()));
  mUndoStack.pop();
}

void UndoRedo::ExecuteRedo()
{
  if (mRedoStack.size() == 0)
  {
    return;
  }

  mRedoStack.top()->Execute();
  mUndoStack.emplace(std::move(mRedoStack.top()));
  mRedoStack.pop();
}

void UndoRedo::InsertCommand(std::unique_ptr<Command> aUndo)
{
  mUndoStack.emplace(std::move(aUndo));
  mRedoStack = std::stack<std::unique_ptr<Command>>();
}
