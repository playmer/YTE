#pragma once

#ifndef YTE_Utilities_String_StringRange_H
#define YTE_Utilities_String_StringRange_H

#include <cstring>
#include "YTE/Utilities/String/String.h"

namespace YTE
{
  class StringRange
  {
  public: 
    using iterator = const char*;


    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////
    //////////////////////////////
    // Constructors
    //////////////////////////////
    inline StringRange()
    {
      mBegin = nullptr;
      mEnd = nullptr;
    }

    inline StringRange(iterator aString)
    {
      mBegin = aString;
      mEnd = mBegin + std::strlen(aString);
    }

    inline StringRange(iterator aBegin, iterator aEnd)
    {
      mBegin = aBegin;
      mEnd = aEnd;
    }

    inline StringRange(iterator aBegin, size_t aLength)
    {
      mBegin = aBegin;
      mEnd = mBegin + aLength;
    }

    inline StringRange(StringRef aString)
    {
      mBegin = aString.Data();
      mEnd = mBegin + aString.Size();
    }

    inline StringRange(std::string &aString)
    {
      mBegin = aString.data();
      mEnd = mBegin + aString.size();
    }


    //////////////////////////////
    // Overloads
    //////////////////////////////
    inline bool operator==(StringRange aString)
    {
      return Size() == aString.Size() &&
             std::strncmp(mBegin, aString.mBegin, Size()) == 0;
    }

    inline bool operator!=(StringRange aString)
    {
      return !(*this == aString);
    }

    inline bool operator==(iterator aString)
    {
      return Size() == std::strlen(aString) &&
        std::strncmp(mBegin, aString, Size()) == 0;
    }

    inline bool operator!=(iterator aString)
    {
      return !(*this == aString);
    }

    inline bool operator==(std::string &aString)
    {
      return Size() == aString.size() &&
        std::strncmp(mBegin, aString.data(), Size()) == 0;
    }

    inline bool operator!=(std::string &aString)
    {
      return !(*this == aString);
    }

    inline bool operator==(StringRef aString)
    {
      return Size() == aString.Size() &&
        std::strncmp(mBegin, aString.Data(), Size()) == 0;
    }

    inline bool operator!=(StringRef aString)
    {
      return !(*this == aString);
    }

    inline char operator[](size_t aIndex) { return *(mBegin + aIndex); }

    //////////////////////////////
    // Getters/Setters
    //////////////////////////////
    inline iterator Front() const {return mBegin; }
    inline iterator Back() const { return mEnd; }


    //////////////////////////////
    // Helpers
    //////////////////////////////
      // These are not checked for safety.
    inline void PopFront() { ++mBegin; }
    inline void PopFront(size_t aLength) { mBegin += aLength; }
    inline void PopBack() { --mEnd; }

    inline bool Contains(iterator aChar) { return mBegin < aChar && aChar < mEnd; }
    int CompareTo(const StringRange &aString) const;

    inline iterator data() const { return mBegin; }
    inline bool empty() const {return mBegin == mEnd; }

      // Costs nothing to doo all of these.
    inline size_t Size() const { return mEnd - mBegin; }
    inline size_t Length() const { return mEnd - mBegin; }
    inline size_t SizeInBytes() const { return mEnd - mBegin; }

    ///////////////////////////////////////
    // Fields
    ///////////////////////////////////////
    iterator mBegin;
    iterator mEnd;
  };
}
#endif