/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
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

      Hook(OwnerType * aOwner)
        : mPrevious(this)
        , mNext(this)
        , mOwner(aOwner)
      {
        if (aOwner == nullptr)
        {
          throw std::exception();
        }
      }

      Hook(Hook &aHook) = delete;

      Hook(Hook &&aHook, OwnerType * aOwner = nullptr)
        : mPrevious(aHook.mPrevious)
        , mNext(aHook.mNext)
        , mOwner(aOwner)
      {
        if (aOwner == nullptr)
        {
          throw std::exception();
        }

        // DebugObjection(aOwner != nullptr, "Hook node moved without supplying an owner", __FUNCTION__);
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

      Hook * mPrevious;
      Hook * mNext;
      OwnerType * mOwner;
    };

    class iterator : public std::iterator<std::bidirectional_iterator_tag, OwnerType>
    {
    public:
      friend class const_iterator;
      friend class IntrusiveList<OwnerType>;

      using pointer = OwnerType*;
      using reference = OwnerType&;

      inline iterator(Hook *aHook = nullptr)
      {
        mCurrent.InsertAfter(*aHook);
      }

      //inline iterator(const iterator &aIterator) = delete;
      inline iterator(const iterator &aIterator)
        : mCurrent(std::move(aIterator.mCurrent))
      {
      
      }

      inline iterator& operator=(const iterator &aIterator) = delete;
      //inline iterator& operator=(const iterator &aIterator)
      //{
      //  mCurrent = aIterator.mCurrent;
      //  return *this;
      //}

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
      for (;;)
      {
        Hook *hook = mHead.mPrevious;
        if (hook == &mHead)
        {
          break;
        }
        hook->Unlink();
      }
    }

    Hook mHead;
  };
}
