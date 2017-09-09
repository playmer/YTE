#include "YTE/Core/EventHandler.hpp"

namespace YTE {
  DefineType(Event)
  {
    YTERegisterType(Event);
  }

  DefineType(EventHandler)
  {
    YTERegisterType(EventHandler);
  }

  std::unordered_map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::mDelegateAllocators;
}