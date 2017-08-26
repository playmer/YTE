#include <stack>

namespace YTE
{
  template <typename T, typename Container = std::deque<T>>
  using Stack = std::stack<T, Container>;
}
