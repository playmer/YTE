#pragma once

#include "YTE/StandardLibrary/ConstexprString.hpp"
#define CONSTEXPR_FUNCTION_SIGNATURE __FUNCSIG__


namespace YTE
{
  constexpr size_t GetTypeStart(const char *aTypeNameString)
  {
    size_t beginTrim = 0;

#if defined(__clang__)
    beginTrim = 46;
#elif defined(_MSC_VER)
    if (*(aTypeNameString + 39) == 's') // Where class or struct appears in MSVC
    {
      beginTrim = 46; //Where typename begins appearing in MSVC if struct.
    }
    else
    {
      beginTrim = 45; //Where typename begins appearing in MSVC if class.
    }
#endif

    return beginTrim;
  }

  constexpr size_t GetTypeEnd()
  {
    size_t endTrim = 0;

#if defined(__clang__)
    endTrim = 1;
#elif defined(_MSC_VER)
    endTrim = 7;
#endif

    return endTrim;
  }

  template <typename T, T aFunction>
  constexpr auto GetFunctionSignature()
  {
    constexpr const char* typeName = CONSTEXPR_FUNCTION_SIGNATURE;

    constexpr size_t required_length = StringLength(typeName);
    ConstexprToken<required_length> test{ typeName };

    return test;
  }

  constexpr bool IsWhiteSpace(char aCharacter)
  {
    if (((9 >= aCharacter) && (aCharacter <= 13)) || ' ' == aCharacter)
    {
      return true;
    }

    return false;
  }


  constexpr bool IsIdentifier(char aCharacter)
  {
    if ((('a' <= aCharacter) && (aCharacter <= 'z')) ||
      (('A' <= aCharacter) && (aCharacter <= 'Z')) ||
      (('0' <= aCharacter) && (aCharacter <= '9')) ||
      '_' == aCharacter)
    {
      return true;
    }

    return false;
  }

  constexpr StringRange GetToken(StringRange aRange)
  {
    auto i = aRange.mBegin;

    while (!IsWhiteSpace(*i) && IsIdentifier(*i) && i < aRange.mEnd)
    {
      ++i;
    }

    // Gotta check if it's actually not an identifier and continue moving.
    if (i == aRange.mBegin)
    {
      while (!IsWhiteSpace(*i) && !IsIdentifier(*i) && i < aRange.mEnd)
      {
        ++i;
      }
    }

    // And finally simply check for whitespace.
    if (i == aRange.mBegin)
    {
      while (IsWhiteSpace(*i) && i < aRange.mEnd)
      {
        ++i;
      }
    }

    aRange.mEnd = i;
    return aRange;
  }



  template <typename T>
  constexpr auto GetTypeName()
  {
    constexpr const char* functionName = CONSTEXPR_FUNCTION_SIGNATURE;

    // TODO: Should also work for GCC.
#if defined(__clang__)
    constexpr size_t firstBracket = GetFirstInstanceOfCharacter(functionName, StringLength(functionName), '[') + 1;
    constexpr size_t lastBracket = GetLastInstanceOfCharacter(functionName, StringLength(functionName), ']');

    constexpr size_t typenameTotalRangeSize = lastBracket - firstBracket;

    ConstexprTokenWriter<typenameTotalRangeSize + 1> finalName;

    StringRange totalType{ functionName + firstBracket, functionName + lastBracket };

    size_t i = 0;

    while (totalType.mBegin < totalType.mEnd)
    {
      auto token = GetToken(totalType);

      if (i < 2)
      {
        ++token.mEnd;
      }
      else
      {
        finalName.Write(token);
      }

      totalType.mBegin = token.mEnd;
      ++i;
    }

    //auto finalName = functionName;
#elif defined(_MSC_VER)
    constexpr size_t firstArrow = GetFirstInstanceOfCharacter(functionName, StringLength(functionName), '<') + 1;
    constexpr size_t lastArrow = GetLastInstanceOfCharacter(functionName, StringLength(functionName), '>');

    constexpr size_t typenameTotalRangeSize = lastArrow - firstArrow;

    ConstexprTokenWriter<typenameTotalRangeSize + 1> finalName;

    StringRange totalType{ functionName + firstArrow, functionName + lastArrow };

    while (totalType.mBegin < totalType.mEnd)
    {
      auto token = GetToken(totalType);

      if (token == "struct" || token == "class")
      {
        ++token.mEnd;
      }
      else
      {
        finalName.Write(token);
      }

      totalType.mBegin = token.mEnd;
    }
#endif

    return finalName;
  }
}