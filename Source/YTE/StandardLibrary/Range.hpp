#pragma once

namespace YTE
{
  template <typename TemplateType>
  class Range
  {
  public:
    Range(const TemplateType &aBegin, const TemplateType &aEnd)
      : mBegin(aBegin), mEnd(aEnd) {};

    bool IsRange() { return mBegin != mEnd; }

    const TemplateType begin() const { return mBegin; }
    const TemplateType end() const { return mEnd; }
    TemplateType begin() { return mBegin; }
    TemplateType end() { return mEnd; }

    typename TemplateType::difference_type size() const { return mEnd - mBegin; }
  protected:
    TemplateType mBegin;
    TemplateType mEnd;
  };
  
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