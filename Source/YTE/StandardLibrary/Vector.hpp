#include <vector>

namespace YTE
{
  template <typename T, typename Allocator = std::allocator<T>>
  using vector = std::vector<T, Allocator>;
};
