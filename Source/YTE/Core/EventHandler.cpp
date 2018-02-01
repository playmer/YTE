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

    auto it = list.mList.begin();
    auto end = list.mList.end();

    while (it != end)
    {
      // While technically the next hook, this hook pointer represents the delegate we're
      // about to invoke.
      auto current = it.NextHook();

      auto &eventDelegate = *it;
      eventDelegate.Invoke(aEvent);

      // We need to check to see if we're reached the end due to some number of events
      // (including the current) removing itself from the list.
      if (it == end)
      {
        break;
      }

      // We check to see if our current next hook is the same. If it isn't it means an
      // event has removed itself, so we redo this loop without incrementing.
      if (false == it.IsNextSame(current))
      {
        continue;
      }

      ++it;
    }

    list.mIterating = false;
  }

  std::map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::cDelegateAllocators;
}