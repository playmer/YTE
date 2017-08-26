#include <deque>

namespace YTE
{
  template <typename T, typename Allocator = std::allocator<T>>
  using Deque = std::deque<T, Allocator>;
}
