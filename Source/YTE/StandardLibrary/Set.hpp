#include <set>

namespace YTE
{
  template <typename T, typename Predicate = std::less<T>, typename Allocator = std::allocator<T>>
  using Set = std::set<T, Predicate, Allocator>;
}
