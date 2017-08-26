#include <unordered_set>

namespace YTE
{
  template <typename T, typename Hasher = std::hash<T>, typename Equality = std::equal_to<T>, typename Allocator = std::allocator<T>>
  using UnorderedMultiset = std::unordered_multiset<T, Hasher, Equality, Allocator>;
}