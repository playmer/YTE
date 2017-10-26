#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTEditor
{
  struct ComponentFactoryHelper
  {
    ComponentFactoryHelper(YTE::Engine *aEngine, YTE::FactoryMap *aComponentFactories);

    template <typename tComponent>
    void CreateComponentFactory();

    YTE::Engine *mEngine;
    YTE::FactoryMap *mComponentFactories;
  };

  void ComponentFactoryInitialization(YTE::Engine *aEngine, YTE::FactoryMap &currComponentFactories);

}