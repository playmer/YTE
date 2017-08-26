#include <forward_list>

namespace YTE
{
  template <typename T, typename Allocator = std::allocator<T>>
  using SinglyLinkedList = forward_list<T, Allocator>;
}
