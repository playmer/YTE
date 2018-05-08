//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef Utilities_hpp
#define Utilities_hpp

#include <cstdint>

#include <memory>
#include <string>
#include <unordered_map>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#define enum_cast(aEnum) static_cast<size_t>(aEnum)
#define YTEUnusedArgument(aArgument) (void)aArgument

namespace YTE
{
  using RSValue = rapidjson::Value;
  using RSDocument = rapidjson::Document;
  using RSStringBuffer = rapidjson::StringBuffer;
  using RSPrettyWriter = rapidjson::PrettyWriter<RSStringBuffer>;
  using RSAllocator = rapidjson::Document::AllocatorType;
  using RSSizeType = rapidjson::SizeType;

  using byte = std::uint8_t;

  using s8 = signed char;
  using s16 = signed short;
  using s32 = signed int;
  using s64 = signed long long;

  using i8 = char;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using u8 = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  inline void runtime_assert(bool aValue, const char *aMessage = "")
  {
    if (false == aValue)
    {
      printf("ASSERT: %s\n", aMessage);

      // Intentionally crashing the program.
      int *base = nullptr;
      *base = 1;
    }
  }

  // We want to be able to use the string literals, this is the only way.
  using namespace std::string_literals;

  enum class StringComparison
  {
    String1Null,     // (We check this first)
    LesserInString1, // The first character that does not match has a lower value in ptr1 than in ptr2
    Equal,
    GreaterInString1,// The first character that does not match has a greater value in ptr1 than in ptr2
    String2Null,     // (We check this Second)
  };

  inline StringComparison StringCompare(const char *aLeft, const char *aRight)
  {
    if (nullptr == aLeft)
    {
      return StringComparison::String1Null;
    }

    if (nullptr == aRight)
    {
      return StringComparison::String2Null;
    }

    auto comparison = std::strcmp(aLeft, aRight);

    if (0 == comparison)
    {
      return StringComparison::Equal;
    }

    if (comparison < 0)
    {
      return StringComparison::LesserInString1;
    }

    // else if (comparison < 0) This is by definition of the domain, no need to check
    {
      return StringComparison::GreaterInString1;
    }
  }


  template <typename ...tArguments>
  inline void UnusedArguments(tArguments const &...)
  {
  }
}

#endif
