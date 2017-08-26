/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-05-15
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_ComponentFactory_h
#define YTE_ComponentFactory_h

#include <memory>

#include "YTE/Event/Events.h"

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  class StringComponentFactory : public BaseEventHandler
  {
  public:
    virtual UniquePointer<Component> MakeComponent(Composition *aOwner,
                                                    Space *aSpace,
                                                    RSValue *aProperties) = 0;

    StringComponentFactory(Engine *aEngine) : mEngine(aEngine) {};

    virtual void Initialize() { };
    virtual ~StringComponentFactory() {};
  protected:
    Engine *mEngine;
  };

  template <typename T>
  class ComponentFactory : public StringComponentFactory
  {
  public:
    UniquePointer<Component> MakeComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties) override
    {
      return std::make_unique<T>(aOwner, aSpace, aProperties);
    }
  
    ComponentFactory(Engine *aEngine) : StringComponentFactory(aEngine) {};
    virtual ~ComponentFactory() { };
  };
}

#endif
