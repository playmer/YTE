/******************************************************************************/
/*!
 * \author Austin Morris
 * \date   2015-04-15
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/Object.hpp"


namespace YTE
{
  DefineType(Event)
  {
    YTERegisterType(Event);
  }

  EventHandler::EventHandler() : mActiveInvokeLoops(0)
  {
  }

  DefineType(EventHandler)
  {
    YTERegisterType(EventHandler);
  }

  bool EventHandler::HandleGoodbyes()
  {
    assert(mActiveInvokeLoops >= 0);

    // don't handle goodbyes if mListeners is being iterated through as
    // changing it during iteration is undefined
    if (mActiveInvokeLoops > 0)
    {
      return false;
    }

    // go through all the goodbyes and remove the each handler from the
    // correct container.
    for (ConversationGoodbye& goodbye : mGoodbyes)
    {
      if (goodbye.mRoleOfHandler == ConversationRole::Listener)
      {
        std::vector<Listener>& listeners = mListeners[goodbye.mEventName];
        for (auto iter = listeners.begin(); iter != listeners.end(); ++iter)
        {
          if (iter->mHandler == goodbye.mHandler)
          {
            delete iter->mCallback;
            listeners.erase(iter);
            break;
          }
        }
      }
      else
      {
        assert(goodbye.mRoleOfHandler == ConversationRole::Speaker);

        std::vector<EventHandler*>& speakers =
          mSpeakers[goodbye.mEventName];
        for (auto iter = speakers.begin(); iter != speakers.end(); ++iter)
        {
          if (*iter == goodbye.mHandler)
          {
            speakers.erase(iter);
            break;
          }
        }
      }
    }

    mGoodbyes.clear();
    return true;
  }

  void EventHandler::Trigger(const std::string &eventName, Event *e)
  {
    HandleGoodbyes();
    ++mActiveInvokeLoops;
                                                                                
		std::vector<Listener>& relevantListeners = mListeners[eventName];
		for (int i = 0; i < relevantListeners.size(); ++i)
    {
			Listener& listener = relevantListeners[i];
      // check if this listener has said goodbye
      // ideally there aren't many goodbyes so this lookup should be fast
      auto listenerGoodbye = std::find_if(mGoodbyes.begin(), mGoodbyes.end(),
        [&listener](ConversationGoodbye& goodbye)
        {
          return (goodbye.mRoleOfHandler == ConversationRole::Listener)
            && (goodbye.mHandler == listener.mHandler);
        }
      );

      // Only callback if the listener hasn't said goodbye
      if (listenerGoodbye == mGoodbyes.end())
      {
        listener.mCallback->Invoke(e);
      }
    }

    --mActiveInvokeLoops;
    HandleGoodbyes();
  }

  void EventHandler::AddGoodbye(ConversationGoodbye aGoodbye)
  {
    // TODO@@@ (Austin): Switch goodbye string to YTE::String or at least move
    // REVIEW@@@ (Austin): Should this goodbye be handled now if it's safe?
    mGoodbyes.emplace_back(aGoodbye);
  }

  void EventHandler::StopListening(
    const std::string &aEventName, EventHandler& aSpeaker)
  {
    // let the speaker know it should forget this handler's callback
    aSpeaker.AddGoodbye(
      ConversationGoodbye(aEventName, this, ConversationRole::Listener));

    // remove the speaker from this handler's speaker container
    std::vector<EventHandler*>& speakers = mSpeakers[aEventName];
    auto speaker = std::find(speakers.begin(), speakers.end(), &aSpeaker);
    DebugObjection(speaker == speakers.end(), 
                "We're deregistering from an event we're not currently listening to. This is continuable, but odd and should be looked at.");

    if (speaker != speakers.end())
    {
      speakers.erase(speaker);
    }
  }

  EventHandler::~EventHandler()
  {
    HandleGoodbyes();

    for (auto iter : mListeners)
    {
      ConversationGoodbye goodbye(iter.first, this,ConversationRole::Speaker);

      for (Listener& listener : mListeners[iter.first])
      {
        listener.mHandler->AddGoodbye(goodbye);
        delete listener.mCallback;
      }
    }

    for (auto iter : mSpeakers)
    {
      ConversationGoodbye goodbye(iter.first,this,ConversationRole::Listener);

      for (EventHandler *handler : mSpeakers[iter.first])
      {
        handler->AddGoodbye(goodbye);
      }
    }
  }
}

