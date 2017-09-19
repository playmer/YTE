#include "YTE/Core/EventHandler.hpp"

namespace YTE {
  YTEDefineType(Event)
  {
    YTERegisterType(Event);
  }

  YTEDefineType(EventHandler)
  {
    YTERegisterType(EventHandler);
  }


  void EventHandler::SendEvent(const std::string &aName, Event *aEvent)
  {
    auto &&list = mEventLists[aName];

    // If this event is already currently being sent on this object, we don't resend it.
    if (list.mIterating)
    {
      return;
    }

    list.mIterating = true;

    for (auto begin = list.mList.begin(), end = list.mList.end();
      begin != end;
      ++begin)
    {
      auto &eventDelegate = *begin;
      eventDelegate.Invoke(aEvent);

      // We need to check to see if we're reached the end due to some number of events
      // (including the current) removing itself from the list.
      if (begin == end)
      {
        break;
      }
    }

    list.mIterating = false;
  }

  std::unordered_map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::mDelegateAllocators;
}