#pragma once

#ifndef YTE_Actions_Group_hpp
#define YTE_Actions_Group_hpp

#include <vector>

#include "YTE/Core/Actions/Action.hpp"

namespace YTE
{
  class ActionGroup
  {
  public:
    friend class ActionSequence;
    YTE_Shared ActionGroup();
    YTE_Shared ActionGroup(const ActionGroup& aGroup);
    YTE_Shared ~ActionGroup();
    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      static_assert(std::is_base_of<Action, T>::value,
                    "Actions must derive from the Action class");

      mActions.emplace_back(new T(aValue, aFinal, aDuration));
    }
    template <typename T>
    void Add(const T& aAction)
    {
      static_assert(std::is_base_of<Action, T>::value,
                    "Actions must derive from the Action class");

      mActions.emplace_back(aAction->Clone());
    }
    YTE_Shared void Call(std::function<void(void)> aCallback);
    YTE_Shared void Delay(float aDuration);
  private:
    YTE_Shared void Init();
    YTE_Shared float Increment(float dt);
    YTE_Shared void operator() ();
    YTE_Shared bool IsDone() const;

    std::vector<Action*> mActions;
  };
}

#endif
