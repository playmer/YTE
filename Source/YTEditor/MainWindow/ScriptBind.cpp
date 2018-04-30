#include "YTE/Meta/Meta.hpp"

#include "YTEditor/MainWindow/ScriptBind.hpp"
#include "YTEditor/MainWindow/SubWindows/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{
  void InitializeYTEditorTypes()
  {
    YTE::InitializeType<YTEditor::LogHandler>();
  }
}

