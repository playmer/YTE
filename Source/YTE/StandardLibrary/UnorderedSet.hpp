#include <unordered_set>

namespace YTE
{
  template <typename T, typename Hasher = std::hash<T>, typename Equality = std::equal_to<T>, typename Allocator = std::allocator<T>>
  using UnorderedSet = std::unordered_set<T, Hasher, Equality, Allocator>;
}