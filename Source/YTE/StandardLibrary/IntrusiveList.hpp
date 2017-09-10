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

      Hook() : previous_(this), next_(this), owner_(nullptr)
      {

      }

      Hook(OwnerType * aOwner) : previous_(this), next_(this), owner_(aOwner)
      {
        if (aOwner == nullptr)
        {
          throw std::exception();
        }
      }

      Hook(Hook &&aHook, OwnerType * aOwner = nullptr)
        : previous_(aHook.previous_), next_(aHook.next_), owner_(aOwner)
      {
        if (aOwner == nullptr)
        {
          throw std::exception();
        }

        // DebugObjection(aOwner != nullptr, "Hook node moved without supplying an owner", __FUNCTION__);
        aHook.Unlink();

        previous_->next_ = this;
        next_->previous_ = this;
      }

      ~Hook()
      {
        RemoveFromList();
      }

      void RemoveFromList()
      {
        next_->previous_ = previous_;
        previous_->next_ = next_;
      }

      void Unlink()
      {
        RemoveFromList();

        previous_ = this;
        next_ = this;
      }

      void InsertAfter(Hook &aHook)
      {
        RemoveFromList();

        previous_ = &aHook;
        next_ = aHook.next_;

        aHook.next_ = this;

        next_->previous_ = this;
      }

      Hook * previous_;
      Hook * next_;
      OwnerType * owner_;
    };

    class iterator : public std::iterator<std::bidirectional_iterator_tag, OwnerType>
    {
    public:
      friend class const_iterator;
      friend class IntrusiveList<OwnerType>;

      using pointer = OwnerType*;
      using reference = OwnerType&;

      inline iterator(Hook *aHook = nullptr) : current_(aHook)
      {

      }

      inline iterator(const iterator &aIterator) : current_(aIterator.current_)
      {

      }

      inline iterator& operator=(const iterator &aIterator)
      {
        current_ = aIterator.current_;
        return *this;
      }

      inline iterator& operator++()
      {
        current_ = current_->next_;
        return *this;
      }

      inline iterator& operator++(int)
      {
        iterator previousIter(*this);
        current_ = current_->next_;
        return *this;
      }

      inline iterator& operator--()
      {
        current_ = current_->previous_;
        return *this;
      }

      inline iterator& operator--(int)
      {
        iterator previousIter(*this);
        current_ = current_->previous_;
        return *this;
      }

      inline bool operator==(const iterator& aIterator)
      {
        return current_ == aIterator.current_;
      }

      inline bool operator!=(const iterator& aIterator)
      {
        return current_ != aIterator.current_;
      }


      inline reference operator*()
      {
        return *current_->owner_;
      }

      inline pointer operator->()
      {
        return current_->owner_;
      }

    private:
      Hook *current_;
    };

    iterator begin()
    {
      return iterator(head_.next_);
    }

    iterator end()
    {
      return iterator(&head_);
    }

    void InsertFront(Hook &aHook)
    {
      aHook.InsertAfter(head_);
    }

    void UnlinkAll()
    {
      for (;;)
      {
        Hook *hook = head_.previous_;
        if (hook == &head_)
        {
          break;
        }
        hook->Unlink();
      }
    }

    Hook head_;
  };
}
