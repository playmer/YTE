#include "YTE/Core/EventHandler.hpp"


std::unordered_map<std::string, BlockAllocator<EventHandler::EventDelegate>> EventHandler::mDelegateAllocators;