#include "YTE/Core/Actions/Action.hpp"

namespace YTE
{
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

  Action * Action::Clone() const
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
    : Action_CRTP(0.1f, this)
    , mHasCalled(false)
    , mCallback(aCallback)
  {
  }

  Action_Callback::Action_Callback(const Action_Callback& aAction)
    : Action_CRTP(0.1f, this)
    , mHasCalled(aAction.mHasCalled)
    , mCallback(aAction.mCallback)
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
}
