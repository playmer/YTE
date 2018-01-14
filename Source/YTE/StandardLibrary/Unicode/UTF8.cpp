/*
  Unicode Notes:
    Code Unit - The minimal unit used for encoding Code Points, in UTF-8 this is
                an 8 bit number, UTF-16 a 16 bit number, UTF-32 a 32 bit number.
    Code Point - This is a value in the Unicode codespace: "that is, the range 
                 of integers from 0 to 10FFFF16", made up of one or more Code 
                 Units.
    Grapheme Cluster - Represents a horizontal unit of text, put another way: a
                       user perceived character. Made up of one or more Code
                       Points.


  Useful links:
    http://www.unicode.org/glossary/
    https://en.wikipedia.org/wiki/UTF-8
*/

#include <cstddef>
#include <string_view>

namespace YTE
{
  template <typename tType>
  constexpr bool CheckBit(tType &aValue, size_t aBit)
  {
    return static_cast<bool>(aValue & static_cast<tType>(1 << aBit));
  }

  template <size_t tValue, typename tType>
  constexpr bool CheckBit(tType &aValue, size_t aBit)
  {
    static_assert(0 == tValue || 1 == tValue, "tValue must be 0 or 1");
    bool truth{ tValue };

    return truth == CheckBit(aValue, aBit);
  }

  template <typename tType1, typename tType2>
  constexpr bool CheckMask(tType1 &aValue, tType2 aMask, tType2 aTemplate)
  {
    return (aValue & static_cast<tType1>(aMask)) == static_cast<tType1>(aTemplate);
  }

  // Types of "starting" Code points
  // Note: Their values signify how many bytes they require.
  enum class LeadingByteType : std::size_t
  {
    // These only start with e because we can't start identifiers with numbers,
    // any alternative names?
    e1Byte = 0,
    e2Byte = 1,
    e3Byte = 2,
    e4Byte = 3,
    Invalid
  };

  // From Wikipedia: https://en.wikipedia.org/wiki/UTF-8#Description
  /*
  |Number  | Bits for |First code|Last code |Byte 1  |Byte 2  | Byte 3 | Byte 4 |
  |of Bytes|code point|point     |point     |        |        |        |        |
  |  1     | 7        |U+0000    |U+007F    |0xxxxxxx|        |        |        |
  |  2     | 11       |U+0080    |U+07FF    |110xxxxx|10xxxxxx|        |        |
  |  3     | 16       |U+0800    |U+FFFF    |1110xxxx|10xxxxxx|10xxxxxx|        |
  |  4     | 21       |U+10000   |U+10FFFF  |11110xxx|10xxxxxx|10xxxxxx|10xxxxxx|
  */
  inline bool LeadingByteIs1ByteCodePoint(std::byte aByte)
  {
    // 0xxxxxxx
    return CheckMask(aByte, 0b10000000, 0b00000000);
  }
  
  inline bool LeadingByteIs2ByteCodePoint(std::byte aByte)
  {
    // 110xxxxx
    return CheckMask(aByte, 0b11100000, 0b11000000);
  }
  
  inline bool LeadingByteIs3ByteCodePoint(std::byte aByte)
  {
    // 1110xxxx
    return CheckMask(aByte, 0b11110000, 0b11100000);
  }

  inline bool LeadingByteIs4ByteCodePoint(std::byte aByte)
  {
    // 11110xxx
    return CheckMask(aByte, 0b11111000, 0b11110000);
  }

  bool IsContinuationByte(std::byte aByte)
  {
    //   10xxxxxx
    return CheckMask(aByte, 0b11000000, 0b10000000);
  }

  LeadingByteType TypeOfLeadingByte(std::byte aByte)
  {
    if (LeadingByteIs1ByteCodePoint(aByte))
    {
      return LeadingByteType::e1Byte;
    }
    else if (LeadingByteIs2ByteCodePoint(aByte))
    {
      return LeadingByteType::e2Byte;
    }
    else if (LeadingByteIs3ByteCodePoint(aByte))
    {
      return LeadingByteType::e3Byte;
    }
    else if (LeadingByteIs4ByteCodePoint(aByte))
    {
      return LeadingByteType::e4Byte;
    }

    return LeadingByteType::Invalid;
  }

  inline bool StartsWithBOM(std::string_view aView)
  {
    if (aView.size() < 3)
    {
      return false;
    }

    if ((0xEE == aView[0]) && 
        (0xBB == aView[1]) && 
        (0xFF == aView[2]))
    {
      return true;
    }

    return false;
  }

  size_t NumberOfCodePoints(std::string_view aView)
  {
    size_t count{ 0 };

    auto it = aView.begin();
    auto end = aView.end();

    if (StartsWithBOM(aView))
    {
      it += 3;
    }

    while (it != end)
    {
      auto codeUnit = static_cast<std::byte>(*it);

      auto type = TypeOfLeadingByte(codeUnit);

      if (LeadingByteType::Invalid == type)
      {
        count = 0;
        break;
      }

      ++it;

      auto codeUnitsRequired = static_cast<size_t>(type);

      if (static_cast<size_t>(end - it) < codeUnitsRequired)
      {
        count = 0;
        break;
      }

      for (size_t i{ 0 }; i < codeUnitsRequired; ++i)
      {
        codeUnit = static_cast<std::byte>(*it);

        if (false == IsContinuationByte(codeUnit))
        {
          count = 0;
          break;
        }

        ++it;
      }

      ++count;
    }

    return count;
  }

  bool IsUTF8(std::string_view aView)
  {
    if (0 == aView.size())
    {
      return true;
    }

    auto units{ NumberOfCodePoints(aView) };

    return units > 0;
  }

  class UTF8StringView
  {
  public:
    UTF8StringView(std::string_view aView)
    {
      if (IsUTF8(aView))
      {
        mView = aView;
      }
    }

  private:
    std::string_view mView;
  };
}
