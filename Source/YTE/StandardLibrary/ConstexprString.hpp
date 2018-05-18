#pragma once

#include <array>

namespace YTE
{
  inline constexpr size_t StringLength(const char *aString)
  {
    size_t i = 0;

    while (*aString != '\0')
    {
      ++i;
      ++aString;
    }

    return i;
  }

  template <size_t tConstSize>
  struct ConstexprToken
  {
    public:
    constexpr ConstexprToken()
    {
    }

    constexpr ConstexprToken(const char *aBegin)
    {
      for (size_t i = 0; i < tConstSize; i++)
      {
        mData[i] = aBegin[i];
      }

      mData[tConstSize] = { '\0' };
    }

    template<size_t tDifferentSize>
    constexpr ConstexprToken(const ConstexprToken<tDifferentSize> &aToken)
    {
      static_assert(tDifferentSize <= tConstSize, "Trying to copy a ConstexprToken to a ConstexprToken of too small a size.");
      for (size_t i = 0; i < tDifferentSize; i++)
      {
        mData[i] = aToken.data()[i];
      }

      mData[tDifferentSize] = '\0';
    }

    constexpr size_t Size() const { return tConstSize; };
    constexpr size_t size() const { return tConstSize; };
    constexpr const char* Data() const { return mData.data(); };
    constexpr const char* data() const { return mData.data(); };

    std::array<char, tConstSize + 1> mData;
  };


  struct StringRange
  {
    constexpr StringRange(const char *aBegin, const char *aEnd)
      : mBegin(aBegin)
      , mEnd(aEnd)
    {

    }

    constexpr StringRange(const char *aBegin)
      : mBegin(aBegin)
      , mEnd(aBegin + StringLength(aBegin))
    {

    }

    constexpr bool BeginsWith(const StringRange &aRight) const
    {
      if (Size() >= aRight.Size())
      {
        for (size_t i = 0; i < aRight.Size(); ++i)
        {
          if (mBegin[i] != aRight.mBegin[i])
          {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    constexpr bool operator==(const StringRange &aRight) const
    {
      if (Size() == aRight.Size())
      {
        for (size_t i = 0; i < Size(); ++i)
        {
          if (mBegin[i] != aRight.mBegin[i])
          {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    constexpr size_t Size() const
    {
      return mEnd - mBegin;
    }

    const char *mBegin;
    const char *mEnd;
  };



  template<size_t tConstSize>
  struct ConstexprTokenWriter : public ConstexprToken<tConstSize>
  {
    constexpr ConstexprTokenWriter()
      : mCurrentIndex(0)
    {
      for (size_t i = 0; i < tConstSize; i++)
      {
        this->mData[i] = 0;
      }

      this->mData[tConstSize] = { '\0' };
    }


    constexpr void Write(StringRange aRange)
    {
      while (aRange.mBegin < aRange.mEnd && mCurrentIndex < tConstSize)
      {
        this->mData[mCurrentIndex] = *aRange.mBegin;
        ++mCurrentIndex;
        ++aRange.mBegin;
      }
    }


    private:
    size_t mCurrentIndex;
  };

  inline constexpr size_t GetLastInstanceOfCharacter(const char *aString, size_t aSize, char aCharacter)
  {
    size_t toReturn = aSize + 1;

    while (aSize != 0)
    {
      if (aString[aSize] == aCharacter)
      {
        toReturn = aSize;
        break;
      }
      --aSize;
    }

    return toReturn;
  }


  inline constexpr size_t GetFirstInstanceOfCharacter(const char *aString, size_t aSize, char aCharacter)
  {
    size_t toReturn = aSize + 1;

    size_t i = 0;

    while (i != aSize)
    {
      if (aString[i] == aCharacter)
      {
        toReturn = i;
        break;
      }
      ++i;
    }

    return toReturn;
  }
}

