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

namespace YTE
{
  /*
  #define YTENewAction(name) \
  class name : public Action_CRTP<name> \
  { \
  public: \
  name(float& aValue, float aFinal, float aDur) \
  : Action(aDur), value(aValue), b(aValue), c(aFinal - b) { } \
  float& value; \
  float b; \
  float c; \
  void Init() override; \
  { \
  if (b != value) { \
  float f = b + c;  \
  b = value; \
  c = f - b;
  } \
  } \
  void operator()(); \
  }; \
  inline void name::operator()();
  */

  class Action
  {
  public:
    friend class ActionGroup;
    explicit Action(float aDur);
    float Increment(float dt);
    bool IsDone() const;
    virtual void Init();
    virtual Action * Clone();
    virtual void operator() ();
    virtual ~Action();
  protected:
    float d;
    float t;
    float Time;
  };

  template <typename T>
  class Action_CRTP : public Action
  {
  public:
    Action_CRTP(float aDur) : Action(aDur)
    {
      static_assert(std::is_base_of<Action_CRTP, T>::value,
        "Action_CRTP is for curiously recurring template pattern use only");
    }
    Action * Clone() { return new T(*reinterpret_cast<T*>(this)); }
  };

  class Action_Callback : public Action_CRTP<Action_Callback>
  {
  public:
    Action_Callback(std::function<void(void)> aCallback);
    void operator() () override;
  private:
    bool mHasCalled;
    std::function<void(void)> mCallback;
  };
}

#endif
