#pragma once

namespace YTE
{
  template <typename TemplateType>
  class ConstRandomAccessIterator;

  template <typename TemplateType>
  class RandomAccessIterator
  {
  public:
    using pointer = TemplateType*;
    using reference = TemplateType&;
    using value_type = TemplateType;
    using size_type = size_t;
    using difference_type = decltype(pointer{} - pointer{});

    inline RandomAccessIterator(pointer aPointer = nullptr)
      : mCurrent(aPointer)
    {

    }

    inline RandomAccessIterator(const RandomAccessIterator &aIterator)
      : mCurrent(aIterator.mCurrent)
    {

    }

    inline RandomAccessIterator& operator=(const RandomAccessIterator &aIterator)
    {
      mCurrent = aIterator.mCurrent;
      return *this;
    }

    inline RandomAccessIterator& operator++()
    {
      ++mCurrent;
      return *this;
    }

    inline RandomAccessIterator operator++(int)
    {
      RandomAccessIterator previousIter{ *this };
      ++mCurrent;
      return previousIter;
    }

    inline RandomAccessIterator& operator--()
    {
      --mCurrent;
      return *this;
    }

    inline RandomAccessIterator operator--(int)
    {
      RandomAccessIterator previousIter{ *this };
      --mCurrent;
      return previousIter;
    }

    operator ConstRandomAccessIterator<TemplateType>();

    inline bool operator==(const RandomAccessIterator &aIterator)
    {
      return mCurrent == aIterator.mCurrent;
    }

    inline bool operator!=(const RandomAccessIterator &aIterator)
    {
      return mCurrent != aIterator.mCurrent;
    }

    inline reference operator*()
    {
      return *mCurrent;
    }

    inline pointer operator->()
    {
      return mCurrent;
    }

    inline difference_type operator-(const RandomAccessIterator &aRight)
    {
      return mCurrent - aRight.mCurrent;
    }

    inline RandomAccessIterator operator-(size_type aRight)
    {
      return mCurrent - aRight;
    }

    inline RandomAccessIterator& operator-=(const difference_type& aDifference)
    {
      mCurrent -= aDifference;
      return *this;
    }

    inline RandomAccessIterator operator+(const difference_type& aDifference) const
    {
      return RandomAccessIterator(mCurrent + aDifference);
    }

    inline RandomAccessIterator& operator+=(const difference_type& aDifference)
    {
      mCurrent += aDifference;
      return *this;
    }

    bool operator<(const RandomAccessIterator& aRight)
    {
      return (mCurrent < aRight.mCurrent);
    }

    bool operator>(const RandomAccessIterator& aRight)
    {
      return (mCurrent > aRight.mCurrent);
    }

    bool operator<=(const RandomAccessIterator& aRight)
    {
      return (mCurrent <= aRight.mCurrent);
    }

    bool operator>=(const RandomAccessIterator& aRight)
    {
      return (mCurrent >= aRight.mCurrent);
    }

  private:
    pointer mCurrent;
  };

  template <typename TemplateType>
  class ConstRandomAccessIterator
  {
  public:
    using pointer = const TemplateType*;
    using reference = const TemplateType&;
    using value_type = const TemplateType;
    using size_type = size_t;

    using difference_type = decltype(pointer{} - pointer{});

    friend class RandomAccessIterator<TemplateType>;

    inline ConstRandomAccessIterator(pointer aPointer = nullptr)
      : mCurrent(aPointer)
    {

    }

    inline ConstRandomAccessIterator(const ConstRandomAccessIterator &aIterator)
      : mCurrent(aIterator.mCurrent)
    {

    }

    inline ConstRandomAccessIterator& operator=(const ConstRandomAccessIterator &aIterator)
    {
      mCurrent = aIterator.mCurrent;
      return *this;
    }

    inline ConstRandomAccessIterator& operator++()
    {
      ++mCurrent;
      return *this;
    }

    inline ConstRandomAccessIterator operator++(int)
    {
      ConstRandomAccessIterator previousIter{ *this };
      ++mCurrent;
      return previousIter;
    }

    inline ConstRandomAccessIterator& operator--()
    {
      --mCurrent;
      return *this;
    }

    inline ConstRandomAccessIterator operator--(int)
    {
      ConstRandomAccessIterator previousIter{ *this };
      --mCurrent;
      return previousIter;
    }

    inline bool operator==(const ConstRandomAccessIterator &aIterator)
    {
      return mCurrent == aIterator.mCurrent;
    }

    inline bool operator!=(const ConstRandomAccessIterator &aIterator)
    {
      return mCurrent != aIterator.mCurrent;
    }

    inline reference operator*()
    {
      return *mCurrent;
    }

    inline pointer operator->()
    {
      return mCurrent;
    }

    inline difference_type operator-(const ConstRandomAccessIterator &aRight)
    {
      return mCurrent - aRight.mCurrent;
    }

    inline ConstRandomAccessIterator operator-(size_type aRight)
    {
      return mCurrent - aRight;
    }

    inline ConstRandomAccessIterator& operator-=(const difference_type& aDifference)
    {
      mCurrent -= aDifference;
      return *this;
    }

    inline ConstRandomAccessIterator operator+(const difference_type& aDifference) const
    {
      return ConstRandomAccessIterator(mCurrent + aDifference);
    }

    inline ConstRandomAccessIterator& operator+=(const difference_type& aDifference)
    {
      mCurrent += aDifference;
      return *this;
    }

    bool operator<(const ConstRandomAccessIterator& aRight)
    {
      return (mCurrent < aRight.mCurrent);
    }

    bool operator>(const ConstRandomAccessIterator& aRight)
    {
      return (mCurrent > aRight.mCurrent);
    }

    bool operator<=(const ConstRandomAccessIterator& aRight)
    {
      return (mCurrent <= aRight.mCurrent);
    }

    bool operator>=(const ConstRandomAccessIterator& aRight)
    {
      return (mCurrent >= aRight.mCurrent);
    }

  private:
    pointer mCurrent;
  };

  template<typename TemplateType>
  RandomAccessIterator<TemplateType>::operator ConstRandomAccessIterator<TemplateType>()
  {
    ConstRandomAccessIterator<TemplateType> it;
    it.mCurrent = mCurrent;

    return it;
  }


  template<typename IteratorType>
  class CountingIterator : public IteratorType
  {
  public:
    inline CountingIterator(const IteratorType &aIterator)
      : IteratorType(aIterator), mCount(0)
    {

    }

    //inline CountingIterator& operator++()
    //{
    //  IteratorType::operator++;
    //  return *this;
    //}
    //
    //inline CountingIterator operator++(int)
    //{
    //  IteratorType::operator++(int);
    //  return previousIter;
    //}

    inline size_t Count()
    {
      return mCount;
    }
  private:
    size_t mCount = 0;
  };
}
