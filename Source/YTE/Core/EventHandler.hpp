/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_EventHandler_hpp
#define YTE_Core_EventHandler_hpp

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "YTE/Core/Object.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"
#include "YTE/StandardLibrary/IntrusiveList.hpp"
#include "YTE/StandardLibrary/BlockAllocator.hpp"

namespace YTE
{
  #define YTEDeclareEvent(aName)      \
  namespace Events                    \
  {                                   \
      extern const std::string aName; \
  }

  #define YTEDefineEvent(aName)         \
  namespace Events                      \
  {                                     \
      const std::string aName = #aName; \
  }

  #define YTERegister(aEventName, aReceiver, aFunction) \
  RegisterEvent<decltype(aFunction), aFunction>(aEventName, aReceiver)

  #define YTEDeregister(aEventName, aReceiver, aFunction) \
  DeregisterEvent<decltype(aFunction), aFunction>(aEventName, aReceiver)

  class Event : public Object
  {
  public:
    YTEDeclareType(Event);
  };

  class EventHandler : public Object
  {
  public:
    YTEDeclareType(EventHandler);
    template <typename tReturn, typename Arg = tReturn>
    struct Binding {};

    template <typename tReturn, typename tObject, typename tEvent>
    struct Binding<tReturn(tObject::*)(tEvent*)>
    {
      using ReturnType = tReturn;
      using tObjectType = tObject;
      using EventType = tEvent;
    };

    using DelegateType = Delegate<void(*)(Event*)>;
    using Invoker = DelegateType::Invoker;

    class EventDelegate : public DelegateType
    {
    public:
      template<typename tObjectType = EventHandler>
      EventDelegate(tObjectType * aObj, Invoker aInvoker, const std::string &aName)
        : DelegateType(aObj, aInvoker)
        , mName(aName)
        , mHook(this)
      {

      }

      EventDelegate(EventDelegate &&aEventDelegate)
        : Delegate(std::move(*this))
        , mName(aEventDelegate.mName)
        , mHook(std::move(aEventDelegate.mHook), this)
      {
      }

      template <typename tFunctionType, tFunctionType aFunction, typename tObjectType, typename tEventType>
      static void Caller(void* aObj, Event* event)
      {
        return (static_cast<tObjectType*>(aObj)->*aFunction)(static_cast<tEventType*>(event));
      }

      template<typename tFunctionType, tFunctionType aFunction, typename tObjectType, typename tEventType>
      static Delegate From(tObjectType * aObj)
      {
        return Delegate(aObj, Caller<tFunctionType, aFunction, tObjectType, tEventType>);
      }

      const std::string &mName;
      IntrusiveList<EventDelegate>::Hook mHook;
    };

    using Deleter = BlockAllocator<EventDelegate>::Deleter;
    using UniqueEvent = std::unique_ptr<EventDelegate, Deleter>;

    template <typename tFunctionType, tFunctionType aFunction, typename tObjectType>
    void RegisterEvent(const std::string &aName, tObjectType* aObject)
    {
      static_assert(std::is_base_of<EventHandler, tObjectType>::value,
                    "tObjectType Must be derived from YTE::EventHandler");
      auto delegate = aObject->template MakeEventDelegate<tFunctionType, aFunction, typename tObjectType>(aName, aObject);
      mEventLists[delegate->mName].mList.InsertFront(delegate->mHook);
    }

    template <typename tFunctionType, tFunctionType aFunction, typename tObjectType>
    void DeregisterEvent(const std::string& aName, tObjectType* aObject)
    {
      static_assert(std::is_base_of<EventHandler, tObjectType>::value, 
                    "tObjectType Must be derived from YTE::EventHandler");
      aObject->template RemoveEventDelegate<tFunctionType, aFunction, tObjectType>(aName, aObject);
    }

    template <typename tFunctionType, tFunctionType aFunction, typename tObjectType>
    EventDelegate* MakeEventDelegate(const std::string& aName, tObjectType* aObject)
    {
      using EventType = typename Binding<tFunctionType>::EventType;

      static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");
      Invoker callerFunction = EventDelegate::Caller<tFunctionType, aFunction, tObjectType, EventType>;

      // TODO: Don't do this this way.
      auto &allocator = mDelegateAllocators[aName];
      auto it = mDelegateAllocators.find(aName);

      auto ptr = allocator.allocate();

      new(ptr) EventDelegate(aObject, callerFunction, it->first);
      mHooks.emplace_back(std::move(UniqueEvent(ptr, allocator.GetDeleter())));
      return mHooks.back().get();
    }

    template <typename tFunctionType, tFunctionType aFunction, typename tObjectType>
    void RemoveEventDelegate(const std::string& aName, tObjectType* aObject)
    {
      using EventType = typename Binding<tFunctionType>::EventType;

      static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");
      Invoker callerFunction = EventDelegate::Caller<tFunctionType, aFunction, tObjectType, EventType>;

      auto it = std::find_if(mHooks.begin(), 
                             mHooks.end(), 
                             [&aName, &aObject, &callerFunction](const UniqueEvent &aEvent)
      {
        return aEvent->mName == aName && 
               aEvent->GetObject() == aObject && 
               aEvent->GetCallerFunction() == callerFunction;
      });

      if (it != mHooks.end())
      {
        mHooks.erase(it);
      }
    }

    void SendEvent(const std::string &aName, Event *aEvent);

    EventHandler() {}
    EventHandler(const EventHandler& aEventHandler) { YTEUnusedArgument(aEventHandler); }
    EventHandler& operator=(const EventHandler& aEventHandler) 
    { 
      YTEUnusedArgument(aEventHandler); 
      return *this; 
    }

  protected:
    struct StdStringRefWrapperEquality
    {
      bool operator()(const std::reference_wrapper<const std::string>& aLeft,
                      const std::reference_wrapper<const std::string>& aRight) const
      {
        return aLeft.get() == aRight.get();
      }

      bool operator()(const std::reference_wrapper<std::string>& aLeft,
                      const std::string &aRight) const
      {
        return aLeft.get() == aRight;
      }

      bool operator()(const std::string &aLeft,
                      const std::reference_wrapper<std::string> &aRight) const
      {
        return aLeft == aRight.get();
      }
    };

    struct EventList
    {
      EventList()
        : mIterating(false)
      {

      }

      bool mIterating;
      IntrusiveList<EventDelegate> mList;
    };

    std::vector<UniqueEvent> mHooks;
    std::unordered_map<std::reference_wrapper<const std::string>, 
                       EventList,
                       std::hash<std::string>, 
                       StdStringRefWrapperEquality> mEventLists;

    static std::unordered_map<std::string, BlockAllocator<EventDelegate>> mDelegateAllocators;
  };
}

#endif
