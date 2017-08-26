#include <queue>

namespace YTE
{
  template <typename T, typename Container = std::vector<T>, typename Predicate = std::less<typename Container::value_type>>
  using PriorityQueue = std::priority_queue<T, Container, Predicate>;
}
