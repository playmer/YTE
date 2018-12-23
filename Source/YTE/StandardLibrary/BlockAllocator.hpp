/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#if !defined(__cpp_lib_memory_resource)
  #error "god has forsaken you child"
#endif

#include <array>
#include <list>

#include "YTE/Meta/Meta.hpp"

namespace YTE
{
  template <typename T, size_t S = 128>
  class BlockAllocator
  {
  public:
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
    using storage_type = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
    using array_type = std::array<storage_type, S>;

    class Deleter
    {
    public:
      Deleter(BlockAllocator<value_type> *aAllocator) : mAllocator(aAllocator)
      {

      }

      void operator()(value_type *aToDelete)
      {
        GenericDestruct<value_type>(reinterpret_cast<byte*>(aToDelete));
        mAllocator->deallocate(aToDelete);
      }

    private:
      BlockAllocator<value_type> *mAllocator;
    };

    Deleter GetDeleter()
    {
      return Deleter(this);
    }

    pointer allocate()
    {
      AllocateIfNeeded();

      return mFreeList.pop_front();
    }

    void deallocate(pointer aPointer)
    {
      mFreeList.push_front(aPointer);
    }

  private:
    class FreeList
    {
      struct Node
      {
        Node *mNext;
      };
    public:

      void push_front(pointer aPointer)
      {
        static_assert(sizeof(value_type) >= sizeof(Node),
          "Type is not large enough to be in a block.");

        auto front = reinterpret_cast<Node*>(aPointer);

        front->mNext = mHead;
        mHead = front;

        ++mSize;
      }

      pointer pop_front()
      {
        auto front = mHead;
        mHead = mHead->mNext;

        --mSize;
        return reinterpret_cast<pointer>(front);
      }

      bool IsEmpty()
      {
        return mHead == nullptr;
      }

      size_type size()
      {
        return mSize;
      }

    private:
      size_type mSize;
      Node *mHead;
    };

    void AllocateIfNeeded()
    {
      if (mFreeList.size() == 0)
      {
        mData.emplace_front(array_type());

        for (auto&i : mData.front())
        {
          pointer j = reinterpret_cast<pointer>(&i);
          mFreeList.push_front(j);
        }
      }
    }

    std::list<array_type> mData;
    FreeList mFreeList;
    size_type mSize;

  };
}
