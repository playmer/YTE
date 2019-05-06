#pragma once

#include <array>
#include <vector>

namespace YTE
{
  template <typename tType>
  class Range
  {
  public:
    Range(tType const& aBegin, tType const& aEnd)
      : mBegin(aBegin), mEnd(aEnd) 
    {
    }

    bool IsRange() { return mBegin != mEnd; }

    const tType begin() const { return mBegin; }
    const tType end() const { return mEnd; }
    tType begin() { return mBegin; }
    tType end() { return mEnd; }

    typename tType::difference_type size() const { return mEnd - mBegin; }
  protected:
    tType mBegin;
    tType mEnd;
  };


  template <typename tType>
  class ContiguousRange
  {
  public:
    ContiguousRange(std::vector<tType>& aContainer)
      : mBegin(&*aContainer.begin()), mEnd(&*aContainer.end())
    {
    }

    ContiguousRange(tType& aValue)
      : mBegin(&aValue), mEnd(&aValue + 1)
    {
    }

    ContiguousRange(tType* aBegin, tType* aEnd)
      : mBegin(aBegin), mEnd(aEnd)
    {
    }

    bool IsRange() { return mBegin != mEnd; }

    tType const* begin() const { return mBegin; }
    tType const* end() const { return mEnd; }
    tType* begin() { return mBegin; }
    tType* end() { return mEnd; }

    typename size_t size() const { return mEnd - mBegin; }
  protected:
    tType* mBegin;
    tType* mEnd;
  };

  template <typename tType>
  ContiguousRange<tType> MakeContiguousRange(std::vector<tType> aContainer)
  {
    return ContiguousRange<tType>(&*aContainer.begin(), &*aContainer.end());
  }

  //template<typename tType, size_t tElementCount>
  //ContiguousRange<tType> MakeContiguousRange(std::array<tType, tElementCount> aContainer)
  //{
  //  return ContiguousRange<tType>(&*aContainer.begin(), &*aContainer.end());
  //}

  template <typename tType>
  ContiguousRange<tType> MakeContiguousRange(tType& aValue)
  {
    return ContiguousRange<tType>(&aValue, &aValue + 1);
  }
  
  namespace detail
  {
    template <typename tReturn, typename Arg = tReturn>
    struct GetReturnType {};

    template <typename tReturn, typename tObject, typename ...tArguments>
    struct GetReturnType<tReturn(tObject::*)(tArguments...)>
    {
      using ReturnType = tReturn;
      using tObjectType = tObject;
    };

    template <typename tIteratorType>
    struct iterator
    {
      iterator(tIteratorType &aIterator)
        : mPair(aIterator, 0)
      {

      }

      iterator& operator++()
      {
        ++mPair.first;
        ++mPair.second;

        return *this;
      }

      auto& operator*()
      {
        return mPair;
      }

      template <typename tOtherIterator>
      bool operator!=(const iterator<tOtherIterator> aRight)
      {
        return mPair.first != aRight.mPair.first;
      }

    private:
      std::pair<tIteratorType, size_t> mPair;
    };

    template <typename tIteratorBegin, typename tIteratorEnd = tIteratorBegin>
    struct range
    {
    public:
      range(tIteratorBegin aBegin, tIteratorEnd aEnd)
        : mBegin(aBegin)
        , mEnd(aEnd)
      {

      }

      bool IsRange() { return mBegin != mEnd; }

      iterator<tIteratorBegin> begin() { return mBegin; }
      iterator<tIteratorEnd> end() { return mEnd; }

    private:
      iterator<tIteratorBegin> mBegin;
      iterator<tIteratorEnd> mEnd;
    };
  }

  template <typename tType>
  auto enumerate(tType &aContainer)
  {
    detail::range<decltype(aContainer.begin()), decltype(aContainer.end())> toReturn{ aContainer.begin(), aContainer.end() };

    return toReturn;
  }

}