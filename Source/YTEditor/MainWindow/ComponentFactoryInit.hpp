#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/CoreComponentFactoryInitilization.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTEditor
{
  void ComponentFactoryInitialization(YTE::Engine *aEngine, YTE::FactoryMap &currComponentFactories);
}