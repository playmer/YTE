#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  YTEDefineType(ActionManager)
  {
    YTERegisterType(ActionManager);
  }


  Action::Action(float aDur)
    : d(aDur)
    , t(0)
    , Time(t)
  {
  }

  float Action::Increment(float dt)
  {
    Time += dt;
    t = Time;
    if (Time > d)
    {
      t = d;
    }
    return Time - d;
  }

  bool Action::IsDone() const
  {
    return Time >= d;
  }

  void Action::Init()
  {
    // NOP
  }

  Action * Action::Clone()
  {
    return new Action(*this);
  }

  void Action::operator()()
  {
    // NOP
  }

  Action::~Action()
  {
    // NOP
  }

  Action_Callback::Action_Callback(std::function<void(void)> aCallback)
    : Action_CRTP(0.1f)
    , mHasCalled(false)
    , mCallback(aCallback)
  {
  }

  void Action_Callback::operator()()
  {
    if (mHasCalled)
    {
      return;
    }
    mHasCalled = true;

    if (!mCallback)
    {
      return;
    }

    mCallback();
  }


  Group::Group()
  {

  }

  Group::Group(const Group& aGroup)
  {
    for (auto& action : aGroup.mActions)
    {
      mActions.push_back(action->Clone());
    }
  }

  Group::~Group()
  {
    for (auto& action : mActions)
    {
      delete action;
    }
  }

  void Group::Call(std::function<void(void)> aCallback)
  {
    mActions.push_back(new Action_Callback(aCallback));
  }

  void Group::Delay(float aDuration)
  {
    mActions.push_back(new Action(aDuration));
  }

  void Group::Init()
  {
    for (auto& action : mActions)
    {
      action->Init();
    }
  }

  float Group::Increment(float dt)
  {
    float t = 0;
    for (auto& action : mActions)
    {
      t = std::max(t, action->Increment(dt));
    }
    return t;
  }

  void Group::operator()()
  {
    for (auto& action : mActions)
    {
      (*action)();
    }
  }

  bool Group::IsDone() const
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

  void ActionSequence::AddGroup(const Group & group)
  {
    mGroups.push(group);
  }

  void ActionSequence::Call(std::function<void(void)> aCallback)
  {
    Group group;
    group.Call(aCallback);
    mGroups.push(std::move(group));
  }

  void ActionSequence::Delay(float aDuration)
  {
    Group group;
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

  ActionManager::ActionManager(Composition * aOwner, RSValue * aProperties)
    : Component(aOwner, nullptr)
  {
    YTEUnusedArgument(aProperties);
  }

  void ActionManager::AddSequence(Composition *aComposition, const ActionSequence &sequence)
  {
    //TODO: Register for delete callback
    mSequences.emplace(aComposition, sequence);
  }

  void ActionManager::Initialize()
  {
  }

  void ActionManager::Update(LogicUpdate *aUpdate)
  {
    std::vector<std::unordered_map<Composition*, ActionSequence>::iterator> finishedSequences;
    for (auto it = mSequences.begin(); it != mSequences.end(); ++it)
    {
      auto& sequence = it->second;
      sequence.Increment(aUpdate->Dt);
      sequence();
      if (sequence.IsDone())
      {
        finishedSequences.push_back(it);
      }
    }

    for (auto& finished : finishedSequences)
    {
      mSequences.erase(finished);
    }
  }

  //void ActionManager::DeletionUpdate(DeletionUpdate *aDeletion)
  //{
  //
  //}
}
