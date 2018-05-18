#pragma once

#ifndef OrderedMultiMap_hpp
#define OrderedMultiMap_hpp

#include <algorithm>
#include <utility>
#include <vector>

#include "YTE/StandardLibrary/Iterator.hpp"
#include "YTE/StandardLibrary/Range.hpp"

namespace YTE
{
  template <typename tKeyType, typename tStoredType>
  class OrderedMultiMap
  {
    public:
    using InternalContainedType = typename std::pair<tKeyType, tStoredType>;
    using ContainedType = typename std::pair<const tKeyType, tStoredType>;
    using ContainerType = typename std::vector<InternalContainedType>;
    using size_type = size_t;// typename ContainerType::size_type;

    using iterator = RandomAccessIterator<ContainedType>;
    using const_iterator = ConstRandomAccessIterator<ContainedType>;

    using counting_iterator = CountingIterator<RandomAccessIterator<ContainedType>>;
    using const_counting_iterator = CountingIterator<RandomAccessIterator<const ContainedType>>;

    using range = Range<iterator>;
    using const_range = Range<const_iterator>;

    template <typename tKeyPossibleType, typename... Arguments>
    iterator Emplace(tKeyPossibleType const& aKey, Arguments &&...aStoredTypeArguments)
    {
      if (mData.size() == 0)
      {
        auto emplacedData = mData.emplace(mData.begin(),
                                          std::forward<const tKeyPossibleType &>(aKey),
                                          std::forward<Arguments &&>(aStoredTypeArguments)...);


        return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
      }

      auto iter = std::upper_bound(mData.begin(),
                                   mData.end(),
                                   aKey,
                                   comparatorUpperBound<tKeyPossibleType, tStoredType>);

      auto emplacedData = mData.emplace(iter,
                                        std::forward<const tKeyPossibleType &>(aKey),
                                        std::forward<Arguments &&>(aStoredTypeArguments)...);

      return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
    }

    private:
    template <typename tKeyPossibleType>
    ContainedType* FindFirstContainedType(tKeyPossibleType const& aKey)
    {
      // Empty optimization
      if (0 == size())
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }

      auto iter = std::lower_bound(mData.begin(),
                                   mData.end(),
                                   aKey,
                                   comparatorLowerBound<tKeyPossibleType, tStoredType>);

      if (iter != mData.end() && iter->first == aKey)
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + (iter - mData.begin())));
      }
      else
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }
    }


    template <typename tKeyPossibleType>
    ContainedType* FindLastContainedType(tKeyPossibleType const& aKey)
    {
      // Empty optimization
      if (0 == size())
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }

      auto iter = std::upper_bound(mData.begin(),
                                   mData.end(),
                                   aKey,
                                   comparatorUpperBound<tKeyPossibleType, tStoredType>);

      if (iter != mData.end() && iter->first == aKey)
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + (iter - mData.begin())));
      }
      else
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }
    }

    public:

    template <typename tKeyPossibleType>
    iterator FindFirst(tKeyPossibleType const& aKey)
    {
      return FindFirstContainedType(aKey);
    }

    template <typename tKeyPossibleType>
    const_iterator FindFirst(tKeyPossibleType const& aKey) const
    {
      return const_cast<const ContainedType*>(FindFirstContainedType(aKey));
    }

    template <typename tKeyPossibleType>
    iterator FindLast(const tKeyPossibleType &aKey)
    {
      return FindLastContainedType(aKey);
    }

    template <typename tKeyPossibleType>
    iterator FindLast(tKeyPossibleType const& aKey) const
    {
      return const_cast<const ContainedType*>(FindLastContainedType(aKey));
    }


    template <typename tKeyPossibleType>
    range FindAll(tKeyPossibleType const& aKey)
    {
      // Empty Optimization
      if (0 == size())
      {
        return range(end(), end());
      }

      auto iter = std::lower_bound(mData.begin(),
                                   mData.end(),
                                   aKey,
                                   comparatorLowerBound<tKeyPossibleType, tStoredType>);

      if (iter != mData.end() && iter->first == aKey)
      {
        iterator first = iterator(reinterpret_cast<ContainedType*>(&(*iter)));
        iterator last = first;

        while (last != end())
        {
          if (last->first == aKey)
          {
            ++last;
          }
          else
          {
            break;
          }
        }

        return range(first, last);
      }
      else
      {
        return range(end(), end());
      }
    }

    template <typename tKeyPossibleType>
    void ChangeKey(iterator aIndex, tKeyPossibleType& aKey)
    {
      auto internalIterator = reinterpret_cast<InternalContainedType*>(&(*aIndex));
      internalIterator->first = aKey;

      InternalContainedType swap = std::move(*internalIterator);

      Erase(aIndex);

      Emplace(std::move(swap.first), std::move(swap.second));
    }

    void Erase(iterator aValueToErase)
    {
      mData.erase(mData.begin() + (aValueToErase - begin()));
    }


    void Erase(range aRangeToErase)
    {
      if (aRangeToErase.IsRange() == false)
      {
        return;
      }

      for (auto it = aRangeToErase.end() - 1; it >= aRangeToErase.begin(); --it)
      {
        mData.erase(mData.begin() + (it - begin()));
      }
    }


    void Erase(iterator aBeginToErase, iterator aEndToErase)
    {
      Erase(range(aBeginToErase, aEndToErase));
    }

    void Clear()
    {
      mData.clear();
    }

    range All()
    {
      return range(begin(), end());
    };

    const_iterator cbegin() const
    {
      return const_cast<const ContainedType*>(reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data())));
    }

    const_iterator cend() const
    {
      return const_cast<const ContainedType*>(reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size())));
    }

    const_iterator begin() const
    {
      return cbegin();
    }

    const_iterator end() const
    {
      return cend();
    }

    iterator begin()
    {
      return iterator(reinterpret_cast<ContainedType*>(mData.data()));
    }

    iterator end()
    {
      return iterator(reinterpret_cast<ContainedType*>(mData.data() + mData.size()));
    }

    template <typename tPossibleKey, typename tPossiblePointer, typename tComparison>
    iterator FindIteratorByPointer(tPossibleKey aKey, tPossiblePointer aValue, tComparison aComparison)
    {
      range rangeOfPossibilities = FindAll(aKey);

      //for (iterator &possible : rangeOfPossibilities)
      for (iterator possible = rangeOfPossibilities.begin(); possible != rangeOfPossibilities.end(); ++possible)
      {
        if (aComparison(possible->second, aValue))
        {
          return possible;
        }
      }

      return end();
    }

    size_type size() const { return mData.size(); }

    protected:
    template <typename tKeyPossibleType, typename tPotentialStoredType>
    static inline bool comparatorLowerBound(std::pair<tKeyType, tPotentialStoredType> const& aLeft, tKeyPossibleType const& aRight)
    {
      bool toReturn = aLeft.first < aRight;
      return toReturn;
    };


    template <typename tKeyPossibleType, typename tPotentialStoredType>
    static inline bool comparatorUpperBound(tKeyPossibleType const& aLeft, std::pair<tKeyType, tPotentialStoredType> const& aRight)
    {
      bool toReturn = aLeft < aRight.first;
      return toReturn;
    };

    ContainerType mData;
  };
}

#endif
