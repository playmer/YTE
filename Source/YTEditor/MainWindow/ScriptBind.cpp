#include "YTE/Meta/Meta.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/ScriptBind.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{
  void InitializeYTEditorTypes()
  {
    YTE::InitializeType<YTEditor::LogHandler>();
  }
}

