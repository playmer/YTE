#pragma once
#ifndef YTE_Utilities_String_String_H
#define YTE_Utilities_String_String_H

#include <string>
#include <iostream> // ostream
#include <atomic>
#include <cstddef>
#include <cstring>

#include "YTE/Platform/TargetDefinitions.hpp"

namespace YTE
{
  class String;

  using StringRef = const String&;

  class String
  {
  public:
    struct StringNode
    {
      ///////////////////////////////////////
      // Functions
      ///////////////////////////////////////
      //////////////////////////////
      // Helpers
      //////////////////////////////
      YTE_Shared void AddReference();
      YTE_Shared void ReleaseReference();

      ///////////////////////////////////////
      // Fields
      ///////////////////////////////////////
      std::atomic<size_t> mReferenceCount;
      size_t mSize;
      size_t mHash;
      char mData[1];

    };

    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////
    //////////////////////////////
    // Constructors
    //////////////////////////////
    YTE_Shared String(const char *aString);
    YTE_Shared String(const char *aString, size_t aSize);
    YTE_Shared String(const std::string &aString);
    YTE_Shared String(const String &aString);
    YTE_Shared String(String &&aString);
    YTE_Shared String(StringNode *aStringNode);
    YTE_Shared String();

    //////////////////////////////
    // Overloads
    //////////////////////////////
    YTE_Shared bool operator==(StringRef aString) const;
    YTE_Shared bool operator!=(StringRef aString) const;
    YTE_Shared char operator[](size_t aIndex);
    YTE_Shared String& operator=(const String &aString);
    YTE_Shared bool operator<(const char *aString) const;
    YTE_Shared bool operator<(const std::string &aString) const;
    YTE_Shared bool operator<(const String &aString) const;
    YTE_Shared bool operator>(const char *aString) const;
    YTE_Shared bool operator>(const std::string &aString) const;
    YTE_Shared bool operator>(const String &aString) const;
    YTE_Shared bool operator==(const char *aString) const;
    YTE_Shared bool operator==(const std::string &aString) const;

    YTE_Shared friend std::ostream & operator<<(std::ostream & aStream, const String &aString);

    //////////////////////////////
    // Helpers
    //////////////////////////////
      // It costs nothing to provide both.
    const char* c_str() const { return mNode->mData; }
    const char* Data() const { return mNode->mData; }
    size_t Hash() const {
      return mNode->mHash; 
    }
    size_t Size() const { return mNode->mSize; }
    bool Empty() const { return mNode->mSize == 0; }
    StringNode* GetNode() const { return mNode; }


      // Modified HashString from Zero
    YTE_Shared static size_t HashString(const char* aString, size_t aLength);
    YTE_Shared static StringNode* AllocateNode(const char *aString, size_t aSize);
    YTE_Shared void Assign(StringNode *aNode);

    YTE_Shared friend bool operator <(const char *lfs, const YTE::String &rhs);
    YTE_Shared friend bool operator <(const std::string &lfs, const YTE::String &rhs);

  private:
    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////

    //////////////////////////////
    // Helpers
    //////////////////////////////
    void AddReference()
    {
      mNode->AddReference();
    }

    void ReleaseReference()
    {
      if (mNode)
      {
        mNode->ReleaseReference();
      }
    }


    ///////////////////////////////////////
    // Fields
    ///////////////////////////////////////
    StringNode *mNode;


    ///////////////////////////////////////
    // Constants
    ///////////////////////////////////////
    static StringNode cEmptyNode;
	};
}

namespace std
{
  template <>
  struct hash<YTE::String>
  {
    using argument_type = YTE::String;
    using result_type = size_t;
    
    size_t operator()(const YTE::String& value) const
    {
      return value.Hash();
    }
  };

  template <>
  struct equal_to<YTE::String>
  {
    using argument_type = YTE::String;
    using result_type = bool;

    bool operator()(const YTE::String& x, const YTE::String& y) const
    {
      return x == y;
    }
  };

} // namespace std

#endif