/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
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
    ActionGroup();
    ActionGroup(const ActionGroup& aGroup);
    ~ActionGroup();
    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      static_assert(std::is_base_of<Action, T>::value,
        "Actions must derive from the Action class");
      mActions.push_back(new T(aValue, aFinal, aDuration));
    }
    template <typename T>
    void Add(const T& aAction)
    {
      static_assert(std::is_base_of<Action, T>::value,
        "Actions must derive from the Action class");
      mActions.push_back(aAction->Clone());
    }
    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
  private:
    void Init();
    float Increment(float dt);
    void operator() ();
    bool IsDone() const;

    std::vector<Action*> mActions;
  };
}

#endif
