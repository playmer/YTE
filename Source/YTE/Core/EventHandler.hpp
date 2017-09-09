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
#include "YTE/Core/Object.hpp"
#include "YTE/StandardLibrary/Delegate.hpp"
#include "YTE/StandardLibrary/IntrusiveList.hpp"
#include "YTE/StandardLibrary/BlockAllocator.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace YTE {
#define DeclareEvent(aName)                  \
namespace Events                             \
{                                            \
    extern const std::string aName;          \
}
#define DefineEvent(aName)                   \
namespace Events                             \
{                                            \
    const std::string aName = #aName;        \
}

#define CONNECT(eventName, receiver, function) \
RegisterEvent<decltype(function), function>(eventName, receiver)

#define DISCONNECT(eventName, receiver, function) \
DeregisterEvent<decltype(function), function>(eventName, receiver)

  class Event : public Object
  {
  public:
    DeclareType(Event);
  };

  class EventHandler : public Object
  {
  public:
    DeclareType(EventHandler);
    template <typename tReturn, typename Arg = tReturn>
    struct Binding {};

    template <typename tReturn, typename tObject, typename tEvent>
    struct Binding<tReturn(tObject::*)(tEvent*)>
    {
      using ReturnType = tReturn;
      using ObjectType = tObject;
      using EventType = tEvent;
    };

    using DelegateType = Delegate<void(*)(Event*)>;
    using Invoker = DelegateType::Invoker;

    class EventDelegate : public DelegateType
    {
    public:
      template<typename ObjectType = EventHandler>
      EventDelegate(ObjectType * aObj, Invoker aInvoker)
        : DelegateType(aObj, aInvoker), mHook(this)
      {

      }

      EventDelegate(EventDelegate &&aEventDelegate)
        : Delegate(std::move(*this)), mHook(std::move(aEventDelegate.mHook), this)
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
      String mName;
      IntrusiveList<EventDelegate>::Hook mHook;
    };

    using Deleter = BlockAllocator<EventDelegate>::Deleter;
    using UniqueEvent = std::unique_ptr<EventDelegate, Deleter>;

    template <typename FunctionType, FunctionType aFunction, typename StringType = String, typename SenderType = EventHandler>
    void Connect(SenderType* aSender, const StringType& aName)
    {
      aSender->template RegisterEvent<FunctionType, aFunction, StringType>(aName, &*this);
    }

    template <typename FunctionType, FunctionType aFunction, typename StringType = String, typename ObjectType = EventHandler>
    void RegisterEvent(const StringType &aName, ObjectType* aObject)
    {
      auto delegate = aObject->template MakeEventDelegate<FunctionType, aFunction, typename StringType, typename ObjectType>(aName, aObject);
      mEventLists[aName].InsertFront(delegate->mHook);
    }

    template <typename FunctionType, FunctionType aFunction, typename StringType = String, typename ObjectType = EventHandler>
    void DeregisterEvent(const StringType& aName, ObjectType* aObject)
    {
      aObject->template RemoveEventDelegate<FunctionType, aFunction, StringType, ObjectType>(aName, aObject);
    }



    template <typename FunctionType, FunctionType aFunction, typename StringType = String, typename ObjectType = EventHandler>
    EventDelegate* MakeEventDelegate(const StringType& aName, ObjectType* aObject)
    {
      using EventType = typename Binding<FunctionType>::EventType;

      static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");
      Invoker callerFunction = EventDelegate::Caller<FunctionType, aFunction, ObjectType, EventType>;
      auto &allocator = mDelegateAllocators[aName];
      auto ptr = allocator.allocate();

      new(ptr) EventDelegate(aObject, callerFunction);
      ptr->mName = aName;
      mHooks.emplace_back(std::move(UniqueEvent(ptr, allocator.GetDeleter())));
      return mHooks.back().get();
    }

    template <typename FunctionType, FunctionType aFunction, typename StringType = String, typename ObjectType = EventHandler>
    void RemoveEventDelegate(const StringType& aName, ObjectType* aObject)
    {
      using EventType = typename Binding<FunctionType>::EventType;

      static_assert(std::is_base_of<Event, EventType>::value, "EventType must be derived from Event");
      Invoker callerFunction = EventDelegate::Caller<FunctionType, aFunction, ObjectType, EventType>;

      for (auto it = mHooks.begin(); it != mHooks.end(); ++it)
      {
        if (it->get()->mName == aName && it->get()->object_ == aObject && it->get()->invoker_ == callerFunction)
        {
          it->get()->mHook.Unlink();
          break;
        }
      }
    }

    template <typename StringType = String>
    void SendEvent(const StringType& aName, Event* aEvent)
    {
      auto it = mEventLists.find(aName);;

      if (it != mEventLists.end())
      {

        for (auto& eventDelegate : it->second)
        {

          eventDelegate.Invoke(aEvent);
        }
      }
    }

    inline EventHandler() {}
    inline EventHandler(const EventHandler& aEventHandler) {}
    inline EventHandler& operator=(const EventHandler& aEventHandler) { return *this; }
  protected:
    std::vector<UniqueEvent> mHooks;
    std::unordered_map<String, IntrusiveList<EventDelegate>> mEventLists;
    static std::unordered_map<std::string, BlockAllocator<EventDelegate>> mDelegateAllocators;
  };
}
#endif
