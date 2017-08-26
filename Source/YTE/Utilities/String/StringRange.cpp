#include "YTE/Utilities/String/StringRange.h"
#include <algorithm>

#undef min

namespace YTE
{
  int StringRange::CompareTo(const StringRange &aString) const
  {
    size_t size = Size();
    size_t otherSize = aString.Size();

    size_t minSize = std::min(size, otherSize);

    int result = strncmp(mBegin, aString.mBegin, minSize);

    if (result != 0)
    {
      return result;
    }

      // If the sizes are the same, strings were equal.
    if (size == otherSize)
    {
      return 0;
    }
      // Left was smaller.
    else if (size < otherSize)
    {
      return -1;
    }
      // Right was smaller.
    else
    {
      return 1;
    }
  }
}