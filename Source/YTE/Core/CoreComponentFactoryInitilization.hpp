/******************************************************************************/
/*!
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2014-04-30
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Component_Initialization_h
#define YTE_Core_Component_Initialization_h

#include <unordered_map>
#include <memory>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  struct ComponentFactoryHelper
  {
    ComponentFactoryHelper(YTE::Engine *aEngine, YTE::FactoryMap *aComponentFactories)
      : mEngine(aEngine)
      , mComponentFactories(aComponentFactories)
    {

    }

    template <typename tComponent>
    void CreateComponentFactory()
    {
      YTE::Type *type = tComponent::GetStaticType();

      auto it = mComponentFactories->Find(type);

      if (it == mComponentFactories->end())
      {
        mComponentFactories->Emplace(type, std::make_unique<YTE::ComponentFactory<tComponent>>(mEngine));
      }
    }

    Engine *mEngine;
    FactoryMap *mComponentFactories;
  };

  void CoreComponentFactoryInitilization(Engine *aEngine, FactoryMap &currComponentFactories);
}

#endif