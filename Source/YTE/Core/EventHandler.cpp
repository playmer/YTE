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

  std::unordered_map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::mDelegateAllocators;
}