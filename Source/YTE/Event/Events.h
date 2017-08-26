/******************************************************************************/
/*!
 * \author Austin Morris
 * \date   2015-04-15
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#pragma once

#ifndef YTE_Events_Events_H
#define YTE_Events_Events_H

#include <unordered_map>
#include <list>
#include <string>
#include <cassert>

#include "YTE/Core/Object.hpp"


namespace YTE
{
  class BaseEventHandler;

  class Event : public Object
  {
  public:
    DeclareType(Event);
    virtual ~Event() {};
  };

  class EventCallback
  {
  public:
    virtual void Invoke(Event* e) = 0;
  };

  class Listener
  {
  public:
    EventCallback *mCallback;
    BaseEventHandler *mHandler;
    Listener(EventCallback *aCallback, BaseEventHandler *aHandler) :
      mCallback(aCallback), mHandler(aHandler) {}
  };

  enum class ConversationRole {Speaker, Listener};

  class ConversationGoodbye
  {
  public:
    std::string mEventName;
    BaseEventHandler *mHandler;
    ConversationRole mRoleOfHandler;
    ConversationGoodbye(const std::string& aEventName,
      BaseEventHandler *aHandler, ConversationRole aRoleOfHandler) :
        mEventName(aEventName), mHandler(aHandler),
        mRoleOfHandler(aRoleOfHandler) {}
  };

  template <typename ClassType, typename EventType>
  class MemberFunctionEventCallback : public EventCallback
  {
  public:
    MemberFunctionEventCallback(ClassType *obThis, 
      void (ClassType::*cb)(EventType*)) 
    {
      callback = cb;
      objectThis = obThis;
    }
  
    void Invoke(Event* e) override
    {
      EventType* actualEvent = dynamic_cast<EventType*>(e);

      // make sure the sent Event matches the Event in the callback
      assert(actualEvent != nullptr);
    
      (objectThis->*callback)(actualEvent);
    }
  private:
    void (ClassType::*callback)(EventType *e);
    ClassType *objectThis;
  };

  class BaseEventHandler : public Object
  {
  public:
    DeclareType(BaseEventHandler);

    BaseEventHandler();

    void Trigger(const std::string &eventName, Event *e);

    void StopListening(const std::string &eventName,
      BaseEventHandler& callbacker);

    template <typename ClassType, typename EventType>
    void RegisterListener(std::string eventName, ClassType& otherObj,
      void (ClassType::*callback)(EventType*))
    {
      RegisterListener(eventName, new
        MemberFunctionEventCallback<ClassType, EventType>(&otherObj, callback), otherObj);
    }

    void RegisterListener(std::string eventName, EventCallback* eventCallback,
      BaseEventHandler& otherObj)
    {
      // if nothing has registered for this Event on this object
      if (mListeners.count(eventName) == 0)
      {
        // make an empty vector
        mListeners[eventName];
      }

      // the listeners listening to this specific type of event
      std::vector<Listener>* listeners = &(mListeners[eventName]);

      listeners->emplace_back(eventCallback, &otherObj);

      // store a pointer to this handler inside the speaker container of the
      // listener, so the listener can end the conversation if it dies first
      std::vector<BaseEventHandler*>& speakers =otherObj.mSpeakers[eventName];
      speakers.emplace_back(this);
    }

    ~BaseEventHandler();
  private:
    void AddGoodbye(ConversationGoodbye aGoodbye);
    bool HandleGoodbyes();

    std::unordered_map<std::string, std::vector<Listener> > mListeners;
    std::unordered_map<std::string, std::vector<BaseEventHandler*>> mSpeakers;

    std::vector<ConversationGoodbye> mGoodbyes;
    int mActiveInvokeLoops;
  };
}

#endif
