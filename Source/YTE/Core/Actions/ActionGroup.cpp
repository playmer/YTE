#include "YTE/Core/Actions/ActionGroup.hpp"

namespace YTE
{
  ActionGroup::ActionGroup()
  {

  }

  ActionGroup::ActionGroup(const ActionGroup& aGroup)
  {
    for (auto& action : aGroup.mActions)
    {
      mActions.push_back(action->Clone());
    }
  }

  ActionGroup::~ActionGroup()
  {
    for (auto& action : mActions)
    {
      delete action;
    }
  }

  void ActionGroup::Call(std::function<void(void)> aCallback)
  {
    mActions.push_back(new Action_Callback(aCallback));
  }

  void ActionGroup::Delay(float aDuration)
  {
    mActions.push_back(new Action(aDuration));
  }

  void ActionGroup::Init()
  {
    for (auto& action : mActions)
    {
      action->Init();
    }
  }

  float ActionGroup::Increment(float dt)
  {
    float t = 0;
    for (auto& action : mActions)
    {
      t = std::max(t, action->Increment(dt));
    }
    return t;
  }

  void ActionGroup::operator()()
  {
    for (auto& action : mActions)
    {
      (*action)();
    }
  }

  bool ActionGroup::IsDone() const
  {
    for (auto& action : mActions)
    {
      if (!action->IsDone())
      {
        return false;
      }
    }
    return true;
  }
}