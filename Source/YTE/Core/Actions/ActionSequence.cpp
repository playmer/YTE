#include "ActionSequence.hpp"
#include "YTE/Core/Actions/ActionSequence.hpp"

namespace YTE
{
  ActionSequence::ActionSequence()
  {
  }

  void ActionSequence::AddGroup(const ActionGroup & group)
  {
    mGroups.push(group);
  }

  void ActionSequence::Call(std::function<void(void)> aCallback)
  {
    ActionGroup group;
    group.Call(aCallback);
    mGroups.push(std::move(group));
  }

  void ActionSequence::Delay(float aDuration)
  {
    ActionGroup group;
    group.Delay(aDuration);
    mGroups.push(std::move(group));
  }

  float ActionSequence::Increment(float dt)
  {
    float t = mGroups.front().Increment(dt);
    while (t > 0 && mGroups.size())
    {
      mGroups.front()();
      mGroups.pop();
      if (mGroups.empty())
      {
        break;
      }
      mGroups.front().Init();
      t = mGroups.front().Increment(t);
    }
    return t;
  }

  void ActionSequence::operator()()
  {
    if (mGroups.size())
    {
      mGroups.front()();
      if (mGroups.front().IsDone())
      {
        mGroups.pop();
        if (!IsDone())
        {
          mGroups.front().Init();
        }
      }
    }
  }

  bool ActionSequence::IsDone() const
  {
    return mGroups.empty();
  }
}