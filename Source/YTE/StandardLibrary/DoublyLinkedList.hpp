#include <list>

namespace YTE
{
  template <typename T, typename Allocator = std::allocator<T>>
  using DoublyLinkedList = list<T, Allocator>;
}
