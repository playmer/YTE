#pragma once

#ifndef OrderedSet_h
#define OrderedSet_h

#include <algorithm>
#include <utility>
#include <vector>

#include "YTE/StandardLibrary/Iterator.hpp"
#include "YTE/StandardLibrary/Range.hpp"

namespace YTE
{
  template <typename KeyType, typename StoredType>
  class OrderedMultiMap
  {
  public:
    using InternalContainedType = typename std::pair<KeyType, StoredType>;
    using ContainedType = typename std::pair<const KeyType, StoredType>;
    using ContainerType = typename std::vector<InternalContainedType>;
    using size_type = size_t;// typename ContainerType::size_type;

    using iterator = RandomAccessIterator<ContainedType>;
    using const_iterator = ConstRandomAccessIterator<ContainedType>;

    using counting_iterator = CountingIterator<RandomAccessIterator<ContainedType>>;
    using const_counting_iterator = CountingIterator<RandomAccessIterator<const ContainedType>>;

    using range = Range<iterator>;
    using const_range = Range<const_iterator>;

    template <typename KeyPossibleType, typename... Arguments>
    iterator Emplace(const KeyPossibleType &aKey, Arguments &&...aStoredTypeArguments)
    {
      if (mData.size() == 0)
      {
        auto emplacedData = mData.emplace(mData.begin(),
          std::forward<const KeyPossibleType &>(aKey),
          std::forward<Arguments &&>(aStoredTypeArguments)...);


        return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
      }

      auto iter = std::upper_bound(mData.begin(),
        mData.end(),
        aKey,
        comparatorUpperBound<KeyPossibleType, StoredType>);

      auto emplacedData = mData.emplace(iter,
        std::forward<const KeyPossibleType &>(aKey),
        std::forward<Arguments &&>(aStoredTypeArguments)...);

      return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
    }

  private:
    template <typename KeyPossibleType>
    ContainedType* FindFirstContainedType(const KeyPossibleType &aKey)
    {
      // Empty optimization
      if (0 == size())
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }

      auto iter = std::lower_bound(mData.begin(),
        mData.end(),
        aKey,
        comparatorLowerBound<KeyPossibleType, StoredType>);

      if (iter != mData.end() && iter->first == aKey)
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + (iter - mData.begin())));
      }
      else
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }
    }


    template <typename KeyPossibleType>
    ContainedType* FindLastContainedType(const KeyPossibleType &aKey)
    {
      // Empty optimization
      if (0 == size())
      {
        return reinterpret_cast<ContainedType*>(const_cast<InternalContainedType*>(mData.data() + size()));
      }

      auto iter = std::upper_bound(mData.begin(),
        mData.end(),
        aKey,
        comparatorUpperBound<KeyPossibleType, StoredType>);

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

    template <typename KeyPossibleType>
    iterator FindFirst(const KeyPossibleType &aKey)
    {
      return FindFirstContainedType(aKey);
    }

    template <typename KeyPossibleType>
    const_iterator FindFirst(const KeyPossibleType &aKey) const
    {
      return const_cast<const ContainedType*>(FindFirstContainedType(aKey));
    }

    template <typename KeyPossibleType>
    iterator FindLast(const KeyPossibleType &aKey)
    {
      return FindLastContainedType(aKey);
    }

    template <typename KeyPossibleType>
    iterator FindLast(const KeyPossibleType &aKey) const
    {
      return const_cast<const ContainedType*>(FindLastContainedType(aKey));
    }


    template <typename KeyPossibleType>
    range FindAll(const KeyPossibleType &aKey)
    {
      // Empty Optimization
      if (0 == size())
      {
        return range(end(), end());
      }

      auto iter = std::lower_bound(mData.begin(),
        mData.end(),
        aKey,
        comparatorLowerBound<KeyPossibleType, StoredType>);

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

    template <typename KeyPossibleType>
    void ChangeKey(iterator aIndex, KeyPossibleType &aKey)
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

    template <typename PossibleKey, typename PossiblePointer, typename Comparison>
    iterator FindIteratorByPointer(PossibleKey aKey, PossiblePointer aValue, Comparison aComparison)
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
    template <typename KeyPossibleType, typename StoredType>
    static inline bool comparatorLowerBound(const std::pair<KeyType, StoredType> &lhs, const KeyPossibleType &rhs)
    {
      bool toReturn = lhs.first < rhs;
      return toReturn;
    };


    template <typename KeyPossibleType, typename StoredType>
    static inline bool comparatorUpperBound(const KeyPossibleType &lhs, const std::pair<KeyType, StoredType> &rhs)
    {
      bool toReturn = lhs < rhs.first;
      return toReturn;
    };

    ContainerType mData;
  };
}

#endif
