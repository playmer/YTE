#include <unordered_map>

namespace YTE
{
  template <typename Key, typename Value, typename Hasher = std::hash<T>, typename Equality = std::equal_to<T>, typename Allocator = std::allocator<T>>
  using UnorderedMultimap = std::unordered_multimap<Key, Value, Hasher, Equality, Allocator>;
}
