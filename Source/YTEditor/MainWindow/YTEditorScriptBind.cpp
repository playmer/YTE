#include "YTE/Meta/Type.hpp"

#include "YTEditorScriptBind.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

void YTEditor::InitializeYTEditorTypes()
{
  YTE::InitializeType<YTE::Translate>();
  YTE::InitializeType<YTE::Scale>();
  YTE::InitializeType<YTE::Rotate>();
}
