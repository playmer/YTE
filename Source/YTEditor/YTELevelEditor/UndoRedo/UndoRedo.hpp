/******************************************************************************/
/*!
\file   UndoRedo.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The undo redo system for the application and the base Command class.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <memory>
#include <stack>

#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{

  class Command
  {
  public:
    Command(OutputConsole *aConsole) : mConsole(aConsole) {}
    ~Command() {}

    virtual void Execute() {};
    virtual void UnExecute() {};

    OutputConsole * mConsole;
  };

  class UndoRedo
  {
  public:
    UndoRedo();
    ~UndoRedo();

    void ExecuteUndo();
    void ExecuteRedo();

    void InsertCommand(std::unique_ptr<Command> aUndo);

  private:

    std::stack<std::unique_ptr<Command>> mUndoStack;
    std::stack<std::unique_ptr<Command>> mRedoStack;

  };
}
