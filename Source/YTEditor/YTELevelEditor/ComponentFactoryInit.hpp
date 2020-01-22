#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/CoreComponentFactoryInitilization.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTEditor/YTEditorMeta.hpp"

namespace YTEditor
{
  YTEditor_Shared void ComponentFactoryInitialization(YTE::Engine *aEngine, YTE::FactoryMap &currComponentFactories);
}