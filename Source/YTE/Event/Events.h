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
  class EventHandler;

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
    EventHandler *mHandler;
    Listener(EventCallback *aCallback, EventHandler *aHandler) :
      mCallback(aCallback), mHandler(aHandler) {}
  };

  enum class ConversationRole {Speaker, Listener};

  class ConversationGoodbye
  {
  public:
    std::string mEventName;
    EventHandler *mHandler;
    ConversationRole mRoleOfHandler;
    ConversationGoodbye(const std::string& aEventName,
      EventHandler *aHandler, ConversationRole aRoleOfHandler) :
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

  class EventHandler : public Object
  {
  public:
    DeclareType(EventHandler);

    EventHandler();

    void SendEvent(const std::string &eventName, Event *e);

    void DISCONNECT(const std::string &eventName,
      EventHandler& callbacker);

    template <typename ClassType, typename EventType>
    void RegisterListener(std::string eventName, ClassType& otherObj,
      void (ClassType::*callback)(EventType*))
    {
      RegisterListener(eventName, new
        MemberFunctionEventCallback<ClassType, EventType>(&otherObj, callback), otherObj);
    }

    void RegisterListener(std::string eventName, EventCallback* eventCallback,
      EventHandler& otherObj)
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
      std::vector<EventHandler*>& speakers =otherObj.mSpeakers[eventName];
      speakers.emplace_back(this);
    }

    ~EventHandler();
  private:
    void AddGoodbye(ConversationGoodbye aGoodbye);
    bool HandleGoodbyes();

    std::unordered_map<std::string, std::vector<Listener> > mListeners;
    std::unordered_map<std::string, std::vector<EventHandler*>> mSpeakers;

    std::vector<ConversationGoodbye> mGoodbyes;
    int mActiveInvokeLoops;
  };
}

#endif
