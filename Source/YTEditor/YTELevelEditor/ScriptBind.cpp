#include "YTE/Meta/Meta.hpp"

#include "YTEditor/YTELevelEditor/ScriptBind.hpp"
#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{
  void InitializeYTEditorTypes()
  {
    YTE::InitializeType<YTEditor::LogHandler>();
  }
}

