/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include "YTE/StandardLibrary/Delegate.hpp"

#include <functional>

// TODO (Evan): Actually finish this class
// Easiest way would probably be to bind a member function
// to the delegate it inherits from
namespace YTE
{
  template <typename Return, typename Arg = Return>
  struct FunctionDelegate : public Delegate<Return, Arg> {};

  template <typename Return, typename ...Arguments>
  class FunctionDelegate<Return(*)(Arguments...)> : public Delegate<Return(*)(Arguments...)>
  {
  public:
    FunctionDelegate(std::function<Return(Arguments...)> aFunction)
      : mFunction(aFunction)
    {
    }

    FunctionDelegate() = default;

    FunctionDelegate(FunctionDelegate &&aFunction) = default;

  protected:
    std::function<Return(Arguments...)> mFunction;
  };
}
