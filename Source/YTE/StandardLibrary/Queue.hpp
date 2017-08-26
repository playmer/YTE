#include <queue>

namespace YTE
{
  template <typename T, typename Container = std::deque<T>>
  using Queue = std::queue<T, Container>;
}
