#include "YTE/Meta/Type.hpp"

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
    YTE::InitializeType<YTEditor::Translate>();
    YTE::InitializeType<YTEditor::Scale>();
    YTE::InitializeType<YTEditor::Rotate>();
  }
}

