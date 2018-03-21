/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#ifndef YTE_Actions_Action_hpp
#define YTE_Actions_Action_hpp
#include <functional>

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
#define YTEDeclareAction(name)                  \
  class name : public Action_CRTP<name>         \
  {                                             \
  public:                                       \
  name(float& aValue, float aFinal, float aDur) \
    : Action_CRTP(aDur, this)                   \
    , mValue(aValue)                            \
    , mB(aValue)                                \
    , mC(aFinal - mB)                           \
  {                                             \
  }                                             \
  name(const name& aAction)                     \
    : Action_CRTP(aAction.mD, this)              \
    , mValue(aAction.mValue)                    \
    , mB(aAction.mB)                            \
    , mC(aAction.mC)                            \
  {                                             \
  }                                             \
  float &mValue;                                \
  float mB;                                     \
  float mC;                                     \
  void Init() override                          \
  {                                             \
    if (mB != mValue)                           \
    {                                           \
      float f = mB + mC;                        \
      mB = mValue;                              \
      mC = f - mB;                              \
    }                                           \
  }                                             \
  void operator()()                             \
  {                                             \
    Ease(mValue, mB, mC, mT, mD);               \
  }                                             \
  static void Ease(float &value,                \
                   float &b,                    \
                   float &c,                    \
                   float &t,                    \
                   float &d);                   \
  }

#define YTEDefineAction(name)     \
  void name::Ease(float &value,   \
                  float &b,       \
                  float &c,       \
                  float &t,       \
                  float &d)

  class Action
  {
  public:
    friend class ActionGroup;
    explicit Action(float aDur);
    float Increment(float dt);
    bool IsDone() const;
    virtual void Init();
    virtual Action * Clone() const;
    virtual void operator() ();
    virtual ~Action();
  protected:
    float mD;
    float mT;
    float mTime;
  };

  template <typename tActionType>
  class Action_CRTP : public Action
  {
  public:
    Action_CRTP(float aDur, tActionType * aDerivedAction)
      : Action(aDur)
      , mDerivedAction(aDerivedAction)
    {
      static_assert(std::is_base_of<Action_CRTP, tActionType>::value,
        "Action_CRTP is for curiously recurring template pattern use only");
    }

    Action * Clone() const override
    { 
      return new tActionType(*reinterpret_cast<tActionType*>(mDerivedAction));
    }
  private:
    tActionType * mDerivedAction;
  };

  class Action_Callback : public Action_CRTP<Action_Callback>
  {
  public:
    Action_Callback(std::function<void(void)> aCallback);
    Action_Callback(const Action_Callback& aAction);
    void operator() () override;
  private:
    bool mHasCalled;
    std::function<void(void)> mCallback;
  };

  namespace Back
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Bounce
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Circ
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Cubic
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Elastic
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Expo
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Linear
  {
    YTEDeclareAction(easeNone);
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Quad
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Quart
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Quint
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

  namespace Sine
  {
    YTEDeclareAction(easeIn);
    YTEDeclareAction(easeOut);
    YTEDeclareAction(easeInOut);
  }

}

#endif
