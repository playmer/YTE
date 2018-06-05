/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Actions_ActionSequence_hpp
#define YTE_Actions_ActionSequence_hpp

#include <queue>

#include "YTE/Core/Actions/ActionGroup.hpp"

namespace YTE
{
  class ActionSequence
  {
  public:
    friend class ActionManager;
    YTE_Shared ActionSequence();
    YTE_Shared void AddGroup(const ActionGroup &group);

    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      static_assert(std::is_base_of<Action, T>::value,
        "Actions must derive from the Action class");
      ActionGroup group;
      group.Add<T>(aValue, aFinal, aDuration);
      mGroups.push(group);
    }

    template <typename T>
    void Add(const T& aAction)
    {
      static_assert(std::is_base_of<Action, T>::value,
        "Actions must derive from the Action class");
      ActionGroup group;
      group.Add<T>(aAction);
      mGroups.push(group);
    }

    YTE_Shared void Call(std::function<void(void)> aCallback);
    YTE_Shared void Delay(float aDuration);
  private:
    YTE_Shared float Increment(float dt);
    YTE_Shared void operator() ();
    YTE_Shared bool IsDone() const;
    std::queue<ActionGroup> mGroups;
  };
}

#endif
