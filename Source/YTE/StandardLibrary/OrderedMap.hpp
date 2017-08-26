#pragma once
#include "YTE/StandardLibrary/OrderedMultiMap.hpp"

namespace YTE
{
  template <typename KeyType, typename StoredType>
  class OrderedMap : public OrderedMultiMap<KeyType, StoredType>
  {
  public:
    using InternalContainedType = typename OrderedMultiMap<KeyType, StoredType>::InternalContainedType;
    using ContainedType = typename OrderedMultiMap<KeyType, StoredType>::ContainedType;
    using ContainerType = typename OrderedMultiMap<KeyType, StoredType>::ContainerType;
    using size_type = typename OrderedMultiMap<KeyType, StoredType>::size_type;

    using iterator = typename OrderedMultiMap<KeyType, StoredType>::iterator;
    using const_iterator = typename OrderedMultiMap<KeyType, StoredType>::const_iterator;
    using counting_iterator = typename OrderedMultiMap<KeyType, StoredType>::counting_iterator;
    using const_counting_iterator = typename OrderedMultiMap<KeyType, StoredType>::const_counting_iterator;

    using range = typename OrderedMultiMap<KeyType, StoredType>::range;
    using const_range = typename OrderedMultiMap<KeyType, StoredType>::const_range;

    template <typename KeyPossibleType, typename... Arguments>
    iterator Emplace(const KeyPossibleType &aKey, Arguments &&...aStoredTypeArguments)
    {
      if (this->mData.size() == 0)
      {
        auto emplacedData = this->mData.emplace(this->mData.begin(),
                                                std::forward<const KeyPossibleType &>(aKey),
                                                std::forward<Arguments &&>(aStoredTypeArguments)...);

        return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
      }

      auto iter = std::lower_bound(this->mData.begin(),
                                   this->mData.end(),
                                   aKey,
                                   OrderedMultiMap<KeyType, StoredType>::template comparatorLowerBound<KeyPossibleType, StoredType>);

      if (iter != this->mData.end() && iter->first == aKey)
      {
        iter = this->mData.erase(iter);
      }

      auto emplacedData = this->mData.emplace(iter,
                                              std::forward<const KeyPossibleType &>(aKey),
                                              std::forward<Arguments &&>(aStoredTypeArguments)...);


      return iterator(reinterpret_cast<ContainedType*>(&(*emplacedData)));
    }

    template <typename KeyPossibleType>
    const_iterator Find(const KeyPossibleType &aKey) const
    {
      return OrderedMultiMap<KeyType, StoredType>::FindFirst(aKey);
    }

    template <typename KeyPossibleType>
    iterator Find(const KeyPossibleType &aKey)
    {
      return OrderedMultiMap<KeyType, StoredType>::FindFirst(aKey);
    }

    template <typename KeyPossibleType>
    StoredType& At(const KeyPossibleType &aKey)
    {
      return OrderedMultiMap<KeyType, StoredType>::FindFirst(aKey)->second;
    }

  private:
    template <typename KeyPossibleType>
    iterator FindFirst(const KeyPossibleType &aKey) = delete;

    template <typename KeyPossibleType>
    iterator FindLast(const KeyPossibleType &aKey) = delete;

    template <typename KeyPossibleType>
    range FindAll(const KeyPossibleType &aKey) = delete;
  };
}