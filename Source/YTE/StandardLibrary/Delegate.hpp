#pragma once

template <typename Return, typename Arg = Return>
struct Delegate {};

template <typename Return, typename ...Arguments>
class Delegate<Return(*)(Arguments...)>
{
  public:
  using FunctionSignature = Return(*)(Arguments...);
  using Invoker = void(*)(void*, Arguments...);


  // None of this for you.
  inline Delegate(const Delegate &aDelegate) = delete;

  template <typename ObjectType, typename FunctionType, FunctionType aFunction, typename EventType>
  static void Caller(void *aObject, Arguments... aArguments)
  {
    (static_cast<ObjectType*>(aObject)->*aFunction)(aArguments...);
  }

  template <typename ObjectType>
  inline Delegate(ObjectType *aObject, Invoker aInvoker)
    : mObject(static_cast<void*>(aObject)),
      mCallerFunction(aInvoker)
  {
  }

  inline Delegate(Delegate &&aDelegate)
    : mObject(aDelegate.mObject),
      mCallerFunction(aDelegate.mCallerFunction)
  {

  }

  inline void Invoke(Arguments... aArguments)
  {
    mCallerFunction(mObject, aArguments...);
  }

protected:

  void *mObject;
  Invoker mCallerFunction;
};
