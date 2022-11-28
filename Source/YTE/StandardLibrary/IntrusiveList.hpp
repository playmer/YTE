#pragma once

#include <iterator>

namespace YTE
{
  template <typename OwnerType>
  class IntrusiveList
  {
  public:
    class Hook
    {
    public:
      Hook& operator=(Hook& aHook) = delete;

      Hook()
        : mPrevious(this)
        , mNext(this)
        , mOwner(nullptr)
      {

      }

      Hook(OwnerType *aOwner)
        : mPrevious(this)
        , mNext(this)
        , mOwner(aOwner)
      {
        DebugObjection(mOwner == nullptr, 
                       "When constructing a Hook, Owner must be valid.");
      }

      Hook(Hook const &aHook) = delete;

      Hook(Hook &&aHook, OwnerType *aOwner)
        : mPrevious(aHook.mPrevious)
        , mNext(aHook.mNext)
        , mOwner(aOwner)
      {
        DebugObjection(mOwner == nullptr,
                       "When constructing a Hook, Owner must be valid.");

        aHook.Unlink();

        mPrevious->mNext = this;
        mNext->mPrevious = this;
      }

      ~Hook()
      {
        RemoveFromList();
      }

      void RemoveFromList()
      {
        mNext->mPrevious = mPrevious;
        mPrevious->mNext = mNext;
      }

      void Unlink()
      {
        RemoveFromList();

        mPrevious = this;
        mNext = this;
      }

      void InsertAfter(Hook &aHook)
      {
        RemoveFromList();

        mPrevious = &aHook;
        mNext = aHook.mNext;

        aHook.mNext = this;

        mNext->mPrevious = this;
      }

      bool Connected()
      {
        return mPrevious != this;
      }

      Hook *mPrevious;
      Hook *mNext;
      OwnerType *mOwner;
    };

    class iterator : public std::iterator<std::bidirectional_iterator_tag, OwnerType>
    {
    public:
      friend class const_iterator;
      friend class IntrusiveList<OwnerType>;

      using pointer = OwnerType*;
      using reference = OwnerType&;

      inline iterator(Hook *aHook)
      {
        DebugObjection(aHook == nullptr,
                       "When constructing an iterator, Hook must be valid.");

        mCurrent.InsertAfter(*aHook);
      }

      inline iterator(const iterator &aIterator)
        : mCurrent(std::move(aIterator.mCurrent))
      {
      
      }

      inline iterator& operator=(const iterator &aIterator) = delete;

      inline iterator& operator++()
      {
        mCurrent.InsertAfter(*mCurrent.mNext);

        return *this;
      }

      inline iterator& operator--()
      {
        mCurrent.InsertAfter(*mCurrent.mPrevious->mPrevious);
        return *this;
      }

      inline iterator operator++(int) = delete;
      inline iterator operator--(int) = delete;

      inline bool operator==(const iterator& aIterator)
      {
        return mCurrent.mNext == &aIterator.mCurrent;
      }

      inline bool operator!=(const iterator& aIterator)
      {
        return mCurrent.mNext != &aIterator.mCurrent;
      }

      inline reference operator*()
      {
        return *mCurrent.mNext->mOwner;
      }

      inline pointer operator->()
      {
        return mCurrent.mNext->mOwner;
      }

      Hook* NextHook()
      {
        return mCurrent.mNext;
      }

      bool IsNextSame(Hook *aNext)
      {
        return mCurrent.mNext == aNext;
      }

    private:
      Hook mCurrent;
    };

    iterator begin()
    {
      return iterator(&mHead);
    }

    iterator end()
    {
      return iterator(mHead.mPrevious);
    }

    void InsertFront(Hook &aHook)
    {
      aHook.InsertAfter(mHead);
    }

    void InsertBack(Hook &aHook)
    {
      aHook.InsertAfter(*mHead.mPrevious);
    }

    void UnlinkAll()
    {
      Hook *hook = mHead.mPrevious;

      while (hook != &mHead)
      {
        hook->Unlink();
        hook = mHead.mPrevious;
      }
    }

    bool Empty()
    {
      return false == mHead.Connected();
    }

    Hook mHead;
  };
}
