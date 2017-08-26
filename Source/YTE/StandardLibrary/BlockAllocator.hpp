#pragma once

#include <allocators>
#include <array>
#include <list>

template <typename T, size_t S = 128>
class BlockAllocator : std::allocator<T>
{
public:
    using value_type = T;
    using pointer = T*;
    /*using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;*/
    using size_type = std::size_t;
    //using difference_type = std::ptrdiff_t;
    using storage_type = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
    using array_type = std::array<storage_type, S>;

    class Deleter
    {
    public:
        Deleter(BlockAllocator<value_type> *aAllocator) : allocator_(aAllocator)
        {
            
        }

        void operator()(value_type *aToDelete)
        {

            allocator_->deallocate(aToDelete);
        }

    private:
        BlockAllocator<value_type> *allocator_;
    };

    Deleter GetDeleter()
    {
        return Deleter(this);
    }

    pointer allocate()
    {
        AllocateIfNeeded();

        return freeList_.pop_front();
    }

    void deallocate(pointer aPointer)
    {
        freeList_.push_front(aPointer);
    }

private:
    class FreeList
    {
        struct Node
        {
            Node *next_;
        };
    public:

        void push_front(pointer aPointer)
        {
            static_assert(sizeof(value_type) >= sizeof(Node),
                "Type is not large enough to be in a block.");

            auto front = reinterpret_cast<Node*>(aPointer);

            front->next_ = head_;
            head_ = front;

            ++size_;
        }

        pointer pop_front()
        {
            auto front = head_;
            head_ = head_->next_;

            --size_;
            return reinterpret_cast<pointer>(front);
        }

        bool IsEmpty()
        {
            return head_ == nullptr;
        }

        size_type size()
        {
            return size_;
        }

    private:
        size_type size_;
        Node *head_;
    };

    void AllocateIfNeeded()
    {
        if (freeList_.size() == 0)
        {
            data_.emplace_front(array_type());

            for (auto&i : data_.front())
            {
                pointer j = reinterpret_cast<pointer>(&i);
                freeList_.push_front(j);
            }
        }
    }

    std::list<array_type> data_;
    FreeList freeList_;
    size_type size_;

};
